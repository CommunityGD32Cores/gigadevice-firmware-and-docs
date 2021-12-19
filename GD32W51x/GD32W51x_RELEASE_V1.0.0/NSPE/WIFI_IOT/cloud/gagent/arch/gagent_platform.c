#include "gagent_platform.h"
#include "gagent_uart.h"
#include "gagent.h"

#include "lwip/netif.h"
#include "wifi_netif.h"


extern u8 airkiss_connection_done;

uint32 newImg2Addr = 0xFFFFFFFF;
uint32 oldImg2Addr = 0xFFFFFFFF;
uint32 userDataAddr = (1024-196)*1024;
OTATYPE otaType;
extern int airkiss_start(void);

u32 GAgent_get_time(void)
{
	return sys_current_time_get();
}

uint32 GAgent_GetDevTime_MS(void)
{
	return sys_current_time_get();
}

uint32 GAgent_GetDevTime_S(void)
{
	u32 tick_ms;

	tick_ms = sys_current_time_get();
	return (uint32)(tick_ms / 1000);
}


void GAgent_DevReset(void)
{
	GAgent_Printf( GAGENT_CRITICAL,"Please restart GAgent !!!\r\n");
}
void GAgent_DevInit( pgcontext pgc )
{

	GAgent_Printf(GAGENT_INFO,"==>GAgent_DevInit\r\n");
}
int32 GAgent_DevGetMacAddress( uint8* szmac )
{
    uint8* tmp;
    uint8 mac[6]={0};

    if(szmac == NULL){
        return -1;
    }

    tmp = (uint8 *)wifi_netif_get_hwaddr();

    mac[0] = tmp[0];
    mac[1] = tmp[1];
    mac[2] = tmp[2];
    mac[3] = tmp[3];
    mac[4] = tmp[4];
    mac[5] = tmp[5];

    sprintf((char *)szmac,"%02X%02X%02X%02X%02X%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    return 0;
}
uint32 GAgent_DevGetConfigData( gconfig *pConfig )
{
    return 0;
}
uint32 GAgent_DevSaveConfigData( gconfig *pConfig )
{
    return 0;
}

int GAgent_DevEraseConfigData()
{
    return 0;
}

int GAgent_DevEraseWiFiConfigData( gconfig *pConfig )
{
	memset(pConfig->wifi_ssid, 0, sizeof(pConfig->wifi_ssid));
	memset(pConfig->wifi_key, 0, sizeof(pConfig->wifi_key));
	memset(pConfig->wifi_bssid, 0, sizeof(pConfig->wifi_bssid));
	pConfig->magicNumber = GAGENT_MAGIC_NUM_CONFIG;

	GAgent_DevSaveConfigData(pConfig);

	return 0;
}

void WifiStatusHandler(int event)
{
}

/****************************************************************
FunctionName    :   GAgent_DRVGetWiFiMode.
Description     :   通过判断pgc->gc.flag  |= XPG_CFG_FLAG_CONNECTED�?
                    是否置位来判断GAgent是否要启用STA �?AP模式.
pgc             :   全局变量.
return          :   1-boot预先设置为STA运行模式
                    2-boot预先设置为AP运行模式
                    3-boot预先设置为STA+AP模式运行
                    >3 保留值�?
Add by Alex.lin     --2015-06-01
****************************************************************/
int8 GAgent_DRVGetWiFiMode( pgcontext pgc )
{
	return 0;
}

int8 GAgent_DRVSetWiFiStartMode(pgcontext pgc, uint32 mode)
{
    return ( pgc->gc.flag |= mode );
}

void DRV_ConAuxPrint(char *buffer, int len, int level)
{
    buffer[len]='\0';
    DEBUGPRINT("%s", buffer);
}

int32 GAgent_OpenUart(int32 BaudRate, int8 number, int8 parity, int8 stopBits, int8 flowControl)
{
    int32 uart_fd = 0;

    uart_fd = serial_open(UART_NAME, BaudRate,number, 'N', stopBits);

    if( uart_fd < 0 ){
	GAgent_Printf(GAGENT_ERROR, "GAgent uart open failed\r\n");
        return -1;
    }

    return uart_fd;
}

void GAgent_LocalDataIOInit( pgcontext pgc )
{
    pgc->rtinfo.local.uart_fd = GAgent_OpenUart(9600, 8, 0, 0, 0);

    while( pgc->rtinfo.local.uart_fd < 0) {
        msleep(1000);
        pgc->rtinfo.local.uart_fd = GAgent_OpenUart(9600, 8, 0, 0, 0);
    }
}

void GAgent_GetMoreInfo(pgcontext pgc)
{
}

int16 GAgent_DRV_WiFi_SoftAPModeStart( const int8* ap_name,const int8 *ap_password,int16 wifiStatus )
{
	return 0;
}

int16 GAgent_DRVWiFi_StationCustomModeStart(int8 *StaSsid,int8 *StaPass,uint16 wifiStatus )
{
	GAgent_DevCheckWifiStatus( WIFI_STATION_CONNECTED,1 );
	GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,1 );

	return wifiStatus;
}
int16 GAgent_DRVWiFi_StationDisconnect()
{
	return RET_SUCCESS;
}

void GAgent_DRVWiFi_APModeStop( pgcontext pgc )
{
	pgc->rtinfo.GAgentStatus &=~ WIFI_MODE_AP;
	GAgent_DevCheckWifiStatus( WIFI_MODE_AP,0 );
}
void GAgent_DRVWiFiPowerScan( pgcontext pgc )
{

}
int8 GAgent_DRVWiFiPowerScanResult( pgcontext pgc )
{
	int rssi = 0;

	return (rssi + 95);
}
void GAgent_DevTick()
{
}
void GAgent_DevLED_Red( uint8 onoff )
{
}

void GAgent_DevLED_Green( uint8 onoff )
{
}



void GAgent_HybridConfigProcess(void *param)
{
}
void GAgent_AirlinkResult( pgcontext pgc );
void GAgent_OpenHybridConfig( int32 timeout_s )
{
	memcpy(pgContextData->gc.wifi_ssid, "test_11n", sizeof("test_11n"));
	memcpy(pgContextData->gc.wifi_key, "12345678", sizeof("12345678"));
	pgContextData->gc.flag |= XPG_CFG_FLAG_CONFIG;
}

void GAgent_OpenAirlink( int32 timeout_s)
{
	airkiss_start();
}

void wifi_set_connection_result(char *ssid, char *pwd)
{
	memset(pgContextData->gc.wifi_ssid,0,sizeof(pgContextData->gc.wifi_ssid));
	memset(pgContextData->gc.wifi_key,0,sizeof(pgContextData->gc.wifi_key));

	memcpy(pgContextData->gc.wifi_ssid,ssid, sizeof(ssid));
	memcpy(pgContextData->gc.wifi_key, pwd, sizeof(pwd));

	pgContextData->gc.flag |=XPG_CFG_FLAG_CONFIG;
}

void GAgent_AirlinkResult( pgcontext pgc )
{
     pgc->gc.flag |=XPG_CFG_FLAG_CONFIG;
}

void GAgent_DRVWiFiStartScan( )
{

}
void GAgent_DRVWiFiStopScan( )
{

}

NetHostList_str * GAgentDRVWiFiScanResult(void)
{
	return NULL;
}

uint32 GAgent_SaveFile( int offset,uint8 *buf,int len )
{
	return RET_SUCCESS;
}
uint32 GAgent_ReadFile( uint16 offset,int8* buf,int32 len )
{
	return RET_SUCCESS;
}

uint32 GAgent_DeleteFile( int32 offset,int32 filelen )
{
	return RET_SUCCESS;
}
uint32 GAgent_SaveUpgradFirmware( int offset,uint8 *buf,int len )
{
	if( OTATYPE_WIFI == otaType )
		return GAgent_SaveFile( offset + newImg2Addr, buf, len );
	else
		return GAgent_SaveFile( offset + userDataAddr, buf, len );
}

uint32 GAgent_ReadOTAFile( uint32 offset,int8* buf,uint32 len )
{
	return  GAgent_ReadFile( offset + userDataAddr, buf, len);
}

uint32 GAgent_DeleteFirmware( int32 offset,int32 filelen )
{
	return GAgent_DeleteFile(offset + userDataAddr, filelen);
}

int32 GAgent_StartUpgrade()
{
	return RET_SUCCESS;
}

void Read_NewImg2Addr()
{
}

int32 GAgent_WIFIOTAByUrl( pgcontext pgc,int8 *szdownloadUrl )
{
	return 0;
}

int8 GAgent_DRVBootConfigWiFiMode(pgcontext pgc)
{
	pgconfig pConfig = &(pgc->gc);

	if (pConfig->magicNumber == GAGENT_MAGIC_NUM_CONFIG
		) {
		GAgent_Printf(GAGENT_INFO, "Device need to be configed\r\n");
		pConfig->magicNumber = GAGENT_MAGIC_NUM;
		pConfig->config_method = 2;
		GAgent_DevSaveConfigData( pConfig );
		GAgent_Config(pConfig->config_method, pgc);
		return 1;
	} else {
		return 0;
	}
}

