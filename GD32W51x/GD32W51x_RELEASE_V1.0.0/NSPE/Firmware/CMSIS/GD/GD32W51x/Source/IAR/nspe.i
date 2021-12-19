/*###ICF### Section handled by ICF editor, don't touch! ****/
#ifdef CONFIG_TZ_ENABLED
#include "config_gdm32.h"
#else
#include "config_gdm32_ntz.h"
#endif
#include "app_cfg.h"
include "..\..\..\..\ROM-EXPORT\symbol\rom_symbol.icf";

/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_0.xml" */
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = RE_FLASH_BASE_NS + RE_IMG_0_NSPE_OFFSET + RE_VTOR_ALIGNMENT;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_FLASH_start__ = RE_FLASH_BASE_NS + RE_IMG_0_NSPE_OFFSET + RE_VTOR_ALIGNMENT;
define symbol __ICFEDIT_region_FLASH_end__   = 0x081FFFFF;
define symbol __ICFEDIT_region_RAM_start__ = RE_SRAM_BASE_NS + RE_NSPE_DATA_START;
define symbol __ICFEDIT_region_RAM_end__   = 0x2006FFFF;

/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x1000;
define symbol __ICFEDIT_size_heap__   = 0x1000;
/**** End of ICF editor section. ###ICF###*/

define memory mem with size = 4G;
define region FLASH_region   = mem:[from __ICFEDIT_region_FLASH_start__   to __ICFEDIT_region_FLASH_end__];
define region RAM_region   = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];

define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

define block DATA with fixed order {
                          rw object wlan_skb.o,
                          rw object ps_mgmt.o,
                          rw object malloc.o,
                          rw,
                        };
define block RAM_CODE {
                          object port.o,
                          object portasm.o,
                          object portasm1.o,
                          object soc_rx.o,
                          object soc_tx.o,
                          section .ram_code,
                      };
define block RAM_CODE_init {
                          ro object port.o,
                          ro object portasm.o,
                          ro object portasm1.o,
                          ro object soc_rx.o,
                          ro object soc_tx.o,
                          section .ram_code_init,
                      };

#ifdef CONFIG_WIFI_HIGH_PERFORMANCE
define block RAM_CODE_HT {
                          object wlan_rx.o,
                          object net_intf.o,
                          object ethernetif.o,
                          object tcp_in.o,
                          object sockets.o,
                          object inet_chksum.o,
                          object ip4_frag.o,
                          object wlan_tx.o,
                      };
define block RAM_CODE_HT_init {
                          ro object wlan_rx.o,
                          ro object net_intf.o,
                          ro object ethernetif.o,
                          ro object tcp_in.o,
                          ro object sockets.o,
                          ro object inet_chksum.o,
                          ro object ip4_frag.o,
                          ro object wlan_tx.o,
                      };
initialize manually {
                          object wlan_rx.o,
                          object net_intf.o,
                          object ethernetif.o,
                          object tcp_in.o,
                          object sockets.o,
                          object inet_chksum.o,
                          object ip4_frag.o,
                          object wlan_tx.o,
                        };
#else
define block RAM_CODE_HT_init {
    };
define block RAM_CODE_HT {
    };
#endif

initialize by copy      { rw };
initialize manually     { object port.o,
                          object portasm.o,
                          object portasm1.o,
                          object soc_rx.o,
                          object soc_tx.o,
                          section .ram_code,
                        };
do not initialize       { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };


place in FLASH_region   { readonly,
                          block RAM_CODE_init,
                          block RAM_CODE_HT_init,
                        };

place at start of RAM_region {
                          block DATA,
                          block RAM_CODE,
                          block RAM_CODE_HT,
                          block CSTACK,
                          block HEAP
                        };