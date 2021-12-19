#ifndef _GAGENT_UART_H_
#define _GAGENT_UART_H_

#define UART_NAME       "uart2"
#define UART_RX_MAX_WAIT_TIME   30
#define UART_RX_BUF_LEN	1024

struct uart_rx_struct {
	int sock_fd;
	u8 sock_created;
	u8 rx_start;
	u8 rx_round;
	u32 read_ptr;
	u32 write_ptr;
	u32 last_update;
	u8 rx_buf[UART_RX_BUF_LEN];
};

int serial_open(char *com_port, int baudrate, int num_bits, char parity, int stop_bits);
int serial_write(int serial_fd, unsigned char *buf, int buf_len);
int serial_read( int serial_fd, unsigned char *buf, int buf_len );

#endif

