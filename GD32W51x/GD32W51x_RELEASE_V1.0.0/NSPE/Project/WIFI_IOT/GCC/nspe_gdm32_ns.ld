#include "nspe_region.h"
#include "app_cfg.h"

/* memory map */
MEMORY
{
  FLASH (rx)            : ORIGIN = LR_IROM1_ADDR, LENGTH = LR_IROM1_SIZE
  RAM (xrw)             : ORIGIN = RW_IRAM1_ADDR, LENGTH = RW_IRAM1_SIZE
}

ENTRY(Reset_Handler)

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 12K;

  .memory_layout :
  {
    KEEP(*wlan_skb.o* (COMMON))
    KEEP(*ps_mgmt.o* (COMMON))
    KEEP(*malloc.o* (.bss.xHeapRegion_0))
#ifdef CONFIG_EXTEND_MEMORY
    KEEP(*malloc.o* (.bss.xHeapRegion_1))
#endif
  } >RAM

/* ISR vectors */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  _sicode = LOADADDR(.code_to_sram);
  .code_to_sram :
  {
    . = ALIGN(4);
    _scode = .;
    KEEP(*port.o* (.text* .rodata*))
    KEEP(*portasm.o* (.text* .rodata*))
    KEEP(*portasm1.o* (.text* .rodata*))
    KEEP(*uart.o* (.text.log_uart_interrupt_handler))
    KEEP(*gd32_it.o* (.text.WLAN_Rx_IRQHandler))
    KEEP(*tasks.o* (.text.xTaskGetSchedulerState))
    KEEP(*tasks.o* (.text.xTaskIncrementTick))
    KEEP(*wrapper_os.o* (.text.sys_memset))

#ifdef CONFIG_WIFI_HIGH_PERFORMANCE
    KEEP(*soc_isr.o* (.text.wlan_interrupt_rx_handler))
    KEEP(*soc_rx.o* (.text* .rodata*))
    KEEP(*wlan_rx.o* (.text* .rodata*))
    KEEP(*net_intf.o* (.text* .rodata*))
    KEEP(*ethernetif.o* (.text* .rodata*))
    KEEP(*tcp_in.o* (.text* .rodata*))
    KEEP(*sockets.o* (.text* .rodata*))
    KEEP(*inet_chksum.o* (.text* .rodata*))

    KEEP(*ip4_frag.o* (.text* .rodata*))
    KEEP(*wlan_tx.o* (.text* .rodata*))
    KEEP(*soc_tx.o* (.text* .rodata*))
#else
    KEEP(*soc_isr.o* (.text.wlan_interrupt_rx_handler))
    KEEP(*soc_rx.o* (.text.soc_rx_tasklet))
    KEEP(*soc_tx.o* (.text.soc_send_xframe))
#endif
    . = ALIGN(4);
    _ecode = .;
  } >RAM AT> FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    /* the symbol _etext will be defined at the end of code section */
    _etext = .;
  } >FLASH

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

   .ARM.extab :
  {
     *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

    .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    } >FLASH

  .ARM.attributes : { *(.ARM.attributes) } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH


  /* provide some necessary symbols for startup file to initialize data */
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    /* the symbol _sdata will be defined at the data section end start */
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    /* the symbol _edata will be defined at the data section end */
    _edata = .;
  } >RAM AT> FLASH

  . = ALIGN(4);
  .bss :
  {
    /* the symbol _sbss will be defined at the bss section start */
    _sbss = .;
    __bss_start__ = _sbss;
    PROVIDE(Image$$RW_IRAM2$$ZI$$Base = __bss_start__);
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    /* the symbol _ebss will be defined at the bss section end */
    _ebss = .;
    __bss_end__ = _ebss;
    PROVIDE(Image$$RW_IRAM2$$ZI$$Limit = __bss_end__);
  } >RAM


 . = ALIGN(8);
  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );

  .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
  {
    . = ALIGN(4);
    PROVIDE( _heap_end = . );
    PROVIDE( Image$$ARM_LIB_STACKHEAP$$ZI$$Base = . );
    . = __stack_size;
    PROVIDE( Image$$ARM_LIB_STACKHEAP$$ZI$$Limit = . );
  } >RAM AT>RAM
}

 /* input sections */
GROUP(libgcc.a libc.a libm.a libnosys.a)

