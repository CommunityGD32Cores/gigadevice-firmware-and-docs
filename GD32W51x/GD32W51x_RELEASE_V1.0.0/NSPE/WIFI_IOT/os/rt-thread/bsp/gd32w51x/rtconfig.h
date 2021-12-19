#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
//#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
//#define RT_USING_OVERFLOW_CHECK
//#define RT_USING_HOOK
//#define RT_USING_IDLE_HOOK
//#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
//#define RT_DEBUG
//#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
//#define RT_USING_EVENT
//#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

//#define RT_USING_MEMPOOL
#define RT_USING_HEAP

/* Using Memory Heap Management for multiple memories applications */
#define RT_USING_MEMHEAP
/* Using Memory Heap Management for general heap interfaces (rt_malloc, etc.) */
#define RT_USING_MEMHEAP_AS_HEAP

/* Using Software Timer */
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO		11

#define RT_TIMER_THREAD_STACK_SIZE	1024

/* Kernel Device Object */
#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE	128
//#define RT_CONSOLE_DEVICE_NAME "uart0"
//#define RT_VER_NUM 0x40001

#define RT_USING_RTC
#ifdef RT_USING_RTC
#define RT_RTC_NAME 				"rtc"
#endif

/* SECTION: finsh, a C-Express shell */
//#define RT_USING_FINSH
///* Using symbol table */
//#define FINSH_USING_SYMTAB

// <section name="LIBC" description="C Runtime library setting" default="always" >
// <bool name="RT_USING_LIBC" description="Using libc library" default="true" />
//#define RT_USING_LIBC

/* SECTION: lwip, a lighwight TCP/IP protocol stack */
//#define RT_USING_LWIP
/* Enable ICMP protocol*/
#define RT_LWIP_ICMP
/* Enable UDP protocol*/
#define RT_LWIP_UDP
/* Enable TCP protocol*/
#define RT_LWIP_TCP
/* Enable DNS */
#define RT_LWIP_DNS
/* Enable DHCP */
#define RT_LWIP_DHCP
/* Enable DEBUG */
#define RT_LWIP_DEBUG

/* the number of simulatenously active TCP connections*/
#define RT_LWIP_TCP_PCB_NUM	5

/* ip address of target*/
#define RT_LWIP_IPADDR0	192
#define RT_LWIP_IPADDR1	168
#define RT_LWIP_IPADDR2	10
#define RT_LWIP_IPADDR3	241

/* gateway address of target*/
#define RT_LWIP_GWADDR0	192
#define RT_LWIP_GWADDR1	168
#define RT_LWIP_GWADDR2	10
#define RT_LWIP_GWADDR3	1

/* mask address of target*/
#define RT_LWIP_MSKADDR0	255
#define RT_LWIP_MSKADDR1	255
#define RT_LWIP_MSKADDR2	255
#define RT_LWIP_MSKADDR3	0

/* tcp thread options */
#define RT_LWIP_TCPTHREAD_PRIORITY		12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE		4
#define RT_LWIP_TCPTHREAD_STACKSIZE		1024

/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY		15
#define RT_LWIP_ETHTHREAD_MBOX_SIZE		4
#define RT_LWIP_ETHTHREAD_STACKSIZE		512

/* TCP sender buffer space */
#define RT_LWIP_TCP_SND_BUF	8192
/* TCP receive window. */
#define RT_LWIP_TCP_WND		8192

//#define CHECKSUM_CHECK_TCP              0
//#define CHECKSUM_CHECK_IP               0
//#define CHECKSUM_CHECK_UDP              0

//#define CHECKSUM_GEN_TCP                0
//#define CHECKSUM_GEN_IP                 0
//#define CHECKSUM_GEN_UDP                0

/* RT-Thread Components */

//#define RT_USING_COMPONENTS_INIT
//#define RT_USING_USER_MAIN
//#define RT_MAIN_THREAD_STACK_SIZE 2048
//#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */


/* Device virtual file system */


/* Device Drivers */

//#define RT_USING_DEVICE_IPC
//#define RT_PIPE_BUFSZ 512
//#define RT_USING_SERIAL
//#define RT_SERIAL_USING_DMA
//#define RT_SERIAL_RB_BUFSZ 64
//#define RT_USING_I2C
//#define RT_USING_PIN
#define RT_USING_PM
//#define RT_USING_SPI
//#define RT_USING_SFUD
//#define RT_SFUD_USING_FLASH_INFO_TABLE

/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */


/* Network */

/* Socket abstraction layer */


/* light weight TCP/IP stack */


/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */


/* peripheral libraries and drivers */


/* miscellaneous packages */


/* sample package */

/* samples: kernel and components samples */


/* example package: hello */

//#define RT_USING_USART0
//#define RT_USING_USART1
//#define RT_USING_SPI0
//#define RT_USING_SPI1
//#define RT_USING_I2C0

#endif
