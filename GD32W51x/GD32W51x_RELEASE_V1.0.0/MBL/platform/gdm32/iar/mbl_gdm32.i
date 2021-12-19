#include "config_gdm32.h"
#include "mbl_region.h"
include "..\..\..\ROM-EXPORT\symbol\rom_symbol.icf";


/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_0.xml" */
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = MBL_CODE_START;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_ROM_start__ = MBL_CODE_START;
define symbol __ICFEDIT_region_ROM_end__   = MBL_CODE_START + MBL_CODE_SIZE - 1;
define symbol __ICFEDIT_region_RAM_SHARED_DATA_start__ = MBL_SHARED_DATA_START;
define symbol __ICFEDIT_region_RAM_SHARED_DATA_end__   = MBL_SHARED_DATA_START + MBL_SHARED_DATA_SIZE - 1;
define symbol __ICFEDIT_region_RAM_start__ = MBL_DATA_START;
define symbol __ICFEDIT_region_RAM_end__   = MBL_DATA_START + 0x10000;
/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x0400;
define symbol __ICFEDIT_size_heap__   = 0x0400;
/**** End of ICF editor section. ###ICF###*/

define memory mem with size = 4G;
define region ROM_region   = mem:[from __ICFEDIT_region_ROM_start__   to __ICFEDIT_region_ROM_end__];
define region RAM_SHARED_DATA_region  = mem:[from __ICFEDIT_region_RAM_SHARED_DATA_start__  to __ICFEDIT_region_RAM_SHARED_DATA_end__];
define region RAM_region   = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];

define block MBL_CMSE_VENEER  { 
                                section Veneer$$CMSE,    
                              };
define block MBL_BUF  { 
                                section .bss object mbl.o,
                              };


define block CSTACK    with alignment = 32, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 32, size = __ICFEDIT_size_heap__     { };

initialize by copy            { readwrite };
                    
do not initialize             { section .noinit };

//place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };

place at start of ROM_region  { 
                                readonly section .intvec,
                                block MBL_CMSE_VENEER,
                                readonly,
                              };
place in RAM_region     { 
                                block MBL_BUF,
                                readwrite,
                                block CSTACK, 
                                block HEAP,
                              };

