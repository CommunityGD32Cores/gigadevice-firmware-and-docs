#include "mbl_region.h"
/* memory map */
MEMORY
{
  FLASH (rx)            : ORIGIN = MBL_CODE_START, LENGTH = MBL_CODE_SIZE
  FLASH_API (rx)        : ORIGIN = MBL_API_START, LENGTH = MBL_API_SIZE
  RAM (xrw)             : ORIGIN = MBL_DATA_START, LENGTH = 64K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .MBL_API_CODE :
  {
    KEEP(*mbl_api.o* (.rodata.mbl_api))
  } > FLASH_API
  __stack_size = DEFINED(__stack_size) ? __stack_size : 12K;

/* ISR vectors */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  memory_layout :
  {
    KEEP(*mbl_flash.o* (.bss.qspi_writing))
  } > RAM

  _sicode = LOADADDR(.code_to_sram);
  .code_to_sram :
  {
    . = ALIGN(4);
    _scode = .;

    KEEP(*mbl_qspi_flash.o* (.text* .rodata*))
    KEEP(*gd32w51x_qspi.o* (.text* .rodata*))

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
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    /* the symbol _ebss will be defined at the bss section end */
    _ebss = .;
    __bss_end__ = _ebss;
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
    . = ALIGN(4);
    PROVIDE( Image$$ARM_LIB_STACKHEAP$$ZI$$Limit = . );
  } >RAM AT>RAM
}

 /* input sections */
GROUP(libgcc.a libc.a libm.a libnosys.a)

