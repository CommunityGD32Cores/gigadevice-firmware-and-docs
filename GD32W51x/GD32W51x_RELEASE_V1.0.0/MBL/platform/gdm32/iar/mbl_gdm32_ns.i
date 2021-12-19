#include "config_gdm32_ntz.h"
#include "mbl_region.h"
include "..\..\..\ROM-EXPORT\symbol\rom_symbol.icf";


/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_0.xml" */
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = MBL_CODE_START;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_FLASH_start__ = MBL_CODE_START;
define symbol __ICFEDIT_region_FLASH_end__   = MBL_CODE_START + MBL_CODE_SIZE - 1;
define symbol __ICFEDIT_region_FLASH_MBL_API_start__ = MBL_API_START;
define symbol __ICFEDIT_region_FLASH_MBL_API_end__   = MBL_API_START + MBL_API_SIZE  - 1;
define symbol __ICFEDIT_region_RAM_start__ = MBL_DATA_START;
define symbol __ICFEDIT_region_RAM_end__   = MBL_DATA_START + 0x10000;
/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x1000;
define symbol __ICFEDIT_size_heap__   = 0x1000;
/**** End of ICF editor section. ###ICF###*/

define memory mem with size = 4G;
define region FLASH_region   = mem:[from __ICFEDIT_region_FLASH_start__   to __ICFEDIT_region_FLASH_end__];
define region FLASH_MBL_API_region  = mem:[from __ICFEDIT_region_FLASH_MBL_API_start__  to __ICFEDIT_region_FLASH_MBL_API_end__];
define region RAM_region   = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];

keep { section .rodata object mbl_api.o };

define block CSTACK    with alignment = 32, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 32, size = __ICFEDIT_size_heap__     { };

define block RAM_CODE { readonly object mbl_qspi_flash.o,  
                        readonly object gd32w51x_qspi.o };
					
define block MBL_API { readonly object mbl_api.o};
                        
initialize by copy { readwrite, 
                     object mbl_qspi_flash.o,  
                     object gd32w51x_qspi.o };
                    
do not initialize  { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };


place in FLASH_region          { readonly,
                                 block RAM_CODE };
place in FLASH_MBL_API_region  { block MBL_API};
place in RAM_region            { readwrite,
                                 block CSTACK, 
                                 block HEAP };

export symbol __ICFEDIT_region_RAM_start__;
export symbol __ICFEDIT_region_RAM_end__;

