#ifndef __GAGENT_PLATFORM_H_
#define __GAGENT_PLATFORM_H_
#include <stdlib.h>
#include <stdio.h>
#include "wrapper_os.h"
#include "gagent_typedef.h"
#include "gagent_net.h"
#include "gagent_uart.h"
#include "malloc.h"
#include "debug_print.h"

/*********macrons*********/

#define WIFI_SOFTVAR    "20170620"
#define WIFI_HARDVER    "FTH1001"

#define CONFIG_LWIP_LAYER 	1

/**********data structure***********/
#define malloc	sys_malloc
#define free    sys_mfree
#define memcpy  sys_memcpy
#define memset 	sys_memset

#define msleep 	sys_ms_sleep

#define GAGENT_PRINT_DATA(level, _TitleString, _HexData, _HexDataLen)			\
	              if(GAgent_loglevelenable( level )==0 )				\
			{									\
				int __i;								\
				u8	*ptr = (u8 *)_HexData;				\
				DEBUGPRINT(_TitleString "--------Len=%d\n\r", _HexDataLen);						\
				for( __i=0; __i<(int)_HexDataLen; __i++ )				\
				{								\
					DEBUGPRINT("%02X ", ptr[__i]);	\
					if (((__i + 1) % 16) == 0)	DEBUGPRINT("\n\r");			\
				}								\
				DEBUGPRINT("\n\r");							\
			}

uint32 GAgent_get_time(void);
void GAgent_DevReset( void );
void GAgent_DevInit( pgcontext pgc );
void GAgent_DevTick( void );
uint32 GAgent_GetDevTime_MS( void );
uint32 GAgent_GetDevTime_S( void );
int32 GAgent_DevGetMacAddress( uint8* szmac );
void GAgent_DevLED_Red( uint8 onoff );
void GAgent_DevLED_Green( uint8 onoff );

uint32 GAgent_DevGetConfigData( gconfig *pConfig );
uint32 GAgent_DevSaveConfigData( gconfig *pConfig);
int GAgent_DevEraseWiFiConfigData( gconfig *pConfig );
void GAgent_LocalDataIOInit( pgcontext pgc );
uint32 GAgent_DeleteFile( int32 offset,int32 filelen );
uint32 GAgent_ReadOTAFile( uint32 offset, int8* buf,uint32 len );
uint32 GAgent_DeleteFirmware( int32 offset,int32 filelen );

void GAgent_OpenHybridConfig( int32 timeout_s );
void GAgent_OpenAirlink( int32 timeout_s );
void GAgent_AirlinkResult( pgcontext pgc );
int8 GAgent_DRVBootConfigWiFiMode( pgcontext pgc );
int8 GAgent_DRVGetWiFiStartMode( pgcontext pgc );
int8 GAgent_DRVSetWiFiStartMode( pgcontext pgc,uint32 mode );
int16 GAgent_DRV_WiFi_SoftAPModeStart( const int8* ap_name,const int8 *ap_password,int16 wifiStatus );
int16 GAgent_DRVWiFi_StationCustomModeStart(int8 *StaSsid,int8 *StaPass,uint16 wifiStatus );
int16 GAgent_DRVWiFi_StationDisconnect( void );
void GAgent_DRVWiFi_APModeStop( pgcontext pgc );
void GAgent_DRVWiFiStartScan( void );
void GAgent_DRVWiFiStopScan( void );
NetHostList_str * GAgentDRVWiFiScanResult(void);
void GAgent_DRVWiFiPowerScan( pgcontext pgc );
int8 GAgent_DRVWiFiPowerScanResult( pgcontext pgc );
int32 GAgent_StartUpgrade( void );
int32 GAgent_WIFIOTAByUrl( pgcontext pgc,int8 *szdownloadUrl );

void DRV_ConAuxPrint( char *buffer, int len, int level );
void wifi_set_connection_result(char *ssid, char *pwd);


/****************************************************************
        FunctionName        :   GAgent_OpenUart.
        Description         :   Open uart.
        BaudRate            :   The baud rate.
        number              :   The number of data bits.
        parity              :   The parity(0: none, 1:odd, 2:evn).
        StopBits            :   The number of stop bits .
        FlowControl         :   support flow control is 1
        return              :   uart fd.
****************************************************************/
int32 GAgent_OpenUart( int32 BaudRate,int8 number,int8 parity,int8 stopBits,int8 flowControl );

#endif

