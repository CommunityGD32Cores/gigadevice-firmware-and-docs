#include "wrapper_os.h"
#include "uart.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "gagent_uart.h"
#include "gagent.h"
//#include "gagent_platform.h"

struct uart_rx_struct uart2_rx;
extern struct netconn *get_socket_netconn(int s);

int serial_open(char *com_port, int baudrate, int num_bits, char parity, int stop_bits)
{
	UART_Init_Type uart_type;

	GAgent_Printf(GAGENT_INFO, "GAgent serial_open, uart name: %s\r\n", com_port);

  	switch (num_bits) {
   	case 7:
    		uart_type.UART_WordLength = UART_WordLength_7b;
    		break;
    	case 8:
    		uart_type.UART_WordLength = UART_WordLength_8b;
    		break;
    	default:
    		uart_type.UART_WordLength = UART_WordLength_8b;
    		break;
  	}

	switch (parity) {
	case 'O':
	  	uart_type.UART_Parity = UART_Parity_Odd;
		break;
	case 'E':
	  	uart_type.UART_Parity = UART_Parity_Even;
		break;
	case 'N':
	  	uart_type.UART_Parity = UART_Parity_No;
		break;
	default:
	  	uart_type.UART_Parity = UART_Parity_No;
		break;
	}

	uart_type.UART_BaudRate = baudrate;

	switch (stop_bits) {
	case 0:
		uart_type.UART_StopBits = UART_StopBits_1;
		break;
	case 1:
		uart_type.UART_StopBits = UART_StopBits_2;
		break;
	default:
		uart_type.UART_StopBits = UART_StopBits_1;
		break;
	}


	uart_init(UART2, &uart_type);

	memset(&uart2_rx, 0, sizeof(struct uart_rx_struct));

	uart2_rx.sock_fd = socket(AF_UNSPEC, SOCK_RAW, 0);
	if (uart2_rx.sock_fd < 0) {
		GAgent_Printf(GAGENT_WARNING, "GAgent serial_open, create socket failed\r\n");
		return -1;
    	}
    	uart2_rx.sock_created = 1;

  	return uart2_rx.sock_fd;
}

static u32 wait_rx_complete(struct uart_rx_struct *uart_rx)
{
	u32 current_time = GAgent_get_time();

	while ((current_time - uart_rx->last_update) < UART_RX_MAX_WAIT_TIME) {
		sys_ms_sleep(5);
		current_time = GAgent_get_time();
	}

	return 0;
}

int serial_read(int serial_fd, unsigned char *buf, int buf_len)
{
	int read_bytes = 0;
	int read_pointer;
	int write_pointer;
	SYS_SR_ALLOC();

	if (buf == NULL || buf_len <= 0 || serial_fd != uart2_rx.sock_fd) {
		GAgent_Printf(GAGENT_WARNING, "GAgent serial_read, buf = 0x%08x buf_len = %d serial_rd = %d\r\n",
				buf, buf_len, serial_fd);
		return 0;
	}

	if (uart2_rx.rx_start) {
		wait_rx_complete(&uart2_rx);
		uart2_rx.rx_start = 0;
		API_EVENT(get_socket_netconn(uart2_rx.sock_fd), NETCONN_EVT_RCVMINUS, 0);
	}

	SYS_CRITICAL_ENTER();

	read_pointer = uart2_rx.read_ptr;
	write_pointer = uart2_rx.write_ptr;

	if (!uart2_rx.rx_round) {
		read_bytes = write_pointer - read_pointer;
	} else {
		read_bytes = (UART_RX_BUF_LEN - read_pointer) + write_pointer;
	}

	if (read_bytes == 0) {
		SYS_CRITICAL_EXIT();
		return read_bytes;
	}

	if (buf_len < read_bytes) {
		read_bytes = buf_len;
	}

	if (!uart2_rx.rx_round) {
		memcpy(buf, (uart2_rx.rx_buf + read_pointer), read_bytes);
	} else {
		if ((read_pointer + read_bytes) > UART_RX_BUF_LEN){
			memcpy(buf, (uart2_rx.rx_buf+read_pointer), (UART_RX_BUF_LEN-read_pointer));
			memcpy(buf+(UART_RX_BUF_LEN-read_pointer), uart2_rx.rx_buf, read_bytes-(UART_RX_BUF_LEN- read_pointer));
		} else {
			memcpy(buf, (uart2_rx.rx_buf+read_pointer), read_bytes);
		}
	}

	if ((read_pointer + read_bytes) >= UART_RX_BUF_LEN){
		uart2_rx.read_ptr = (read_pointer + read_bytes) - UART_RX_BUF_LEN;
		uart2_rx.rx_round = 0;
	} else {
		uart2_rx.read_ptr = read_pointer + read_bytes;
	}

	SYS_CRITICAL_EXIT();

	return read_bytes;
}

int serial_write(int serial_fd, unsigned char *buf, int buf_len)
{
	int data_len = buf_len;

	if (buf_len <= 0) {
		GAgent_Printf(GAGENT_WARNING, "GAgent serial_write, buf_len = %d\r\n", buf_len);
		return 0;
	}

	while (1) {
		while (uart_get_status(UART2, UART_LSR_THR_Empty) != 1);
		uart_send_data(UART2, *buf++);
		data_len--;
		if (data_len == 0) {
			break;
		}
	}

	return buf_len;
}

#if 0
void uart2_interrupt_handler(void)
{
	if (uart_get_intr(UART2, UART_IID_Rx_Line_Status)) {
		if (uart_get_status(UART2, UART_LSR_Frame_Err | UART_LSR_Parity_Err | UART_LSR_Overrun_Err)) {

		}
	}

	if (uart_get_intr(UART2, UART_IID_Rx_Data_Available) == 1) {
		if (uart2_rx.rx_start == 0) {
			if (uart2_rx.sock_created) {
				API_EVENT(get_socket_netconn(uart2_rx.sock_fd), NETCONN_EVT_RCVPLUS, 0);
			}
			uart2_rx.rx_start = 1;
		}
		uart2_rx.rx_buf[uart2_rx.write_ptr++] = uart_recv_data(UART2);
		if (uart2_rx.write_ptr > (UART_RX_BUF_LEN -1)) {
			uart2_rx.write_ptr = 0;
			uart2_rx.rx_round = 1;
		}
		if (uart2_rx.rx_round && (uart2_rx.write_ptr > uart2_rx.read_ptr)) {
			uart2_rx.read_ptr = uart2_rx.write_ptr;
		}
		uart2_rx.last_update = sys_current_time_get();
	}
}
#endif
