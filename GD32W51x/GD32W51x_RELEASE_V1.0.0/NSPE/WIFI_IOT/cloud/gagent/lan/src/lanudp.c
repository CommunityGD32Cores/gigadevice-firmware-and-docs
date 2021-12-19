#include "gagent.h"
#include "lan.h"
#include "lanudp.h"
#include "3rdlan.h"
#include "mqttlib.h"

static void Lan_dispatchUdpData(pgcontext pgc, struct sockaddr_t *paddr,ppacket prxBuf, int32 recLen);
static int32 Lan_udpOnBoarding(pgcontext pgc, uint8 *buf);
static void LAN_onDiscoverAck(pgcontext pgc, uint8 *ptxBuf, struct sockaddr_t *paddr);
static void LAN_onBoardingAck(pgcontext pgc, uint8 *ptxBuf, struct sockaddr_t *paddr);

/****************************************************************
        FunctionName        :   GAgent_LANInit.
        Description         :   init clients socket and create tcp/udp server.
        Add by Will.zhou     --2015-03-10
****************************************************************/
void Lan_CreateUDPServer(int32 *pFd, uint16 udp_port)
{
    *pFd = GAgent_CreateUDPServer( udp_port );
}
/****************************************************************
        FunctionName        :   Lan_CreateUDPBroadCastServer.
        Description         :   create udp BroadCastServer.
        Add by Will.zhou     --2015-03-10
****************************************************************/
#if 0
struct sockaddr_t Lan_CreateUDPBroadCastServer(int32 *pFd, uint16 udp_port )
{
    struct sockaddr_t addr;
    *pFd = GAgent_CreateUDPBroadCastServer( udp_port, &addr );
    return addr;
}
#else
void Lan_CreateUDPBroadCastServer(int32 *pFd, uint16 udp_port, struct sockaddr_t *paddr)
{
    *pFd = GAgent_CreateUDPBroadCastServer( udp_port, paddr);
}
#endif
void Lan_udpDataHandle(pgcontext pgc, ppacket prxBuf, int32 len)
{
    struct sockaddr_t addr;
    int addrLen = sizeof(struct sockaddr_t);
    int32 recLen=0;
    if( pgc->ls.udpServerFd>0 )
    {
        if(FD_ISSET(pgc->ls.udpServerFd, &(pgc->rtinfo.readfd)))
        {
            resetPacket(prxBuf);
            recLen = Socket_recvfrom(pgc->ls.udpServerFd, prxBuf->phead, len,
                &addr, (socklen_t *)&addrLen);
//            GAgent_Printf( GAGENT_INFO,"UDP RECEIVE LEN = %d error=%d ",recLen,errno );
            if( recLen<=0 )
            {
                GAgent_Printf( GAGENT_ERROR,"Need to Restart Lan udp Server!!!");
                Lan_CreateUDPServer( &(pgc->ls.udpServerFd), LAN_UDP_SERVER_PORT );
                return ;
            }
            Lan_dispatchUdpData(pgc, &addr, prxBuf , recLen);
        }
    }

}

/****************************************************************
        FunctionName        :   Lan_dispatchUdpData.
        Description         :   dispatchUdpData.
        Add by Will.zhou     --2015-03-10
****************************************************************/
static void Lan_dispatchUdpData(pgcontext pgc, struct sockaddr_t *paddr,
            ppacket prxBuf, int32 recLen)
{
    int32 packetLen;
    int32 bytesOfLen;
    int32 dataLen;
    uint32 offsetPayload;
    uint16 cmd;
    uint8 *buf;

    GAGENT_PRINT_DATA(GAGENT_INFO, "\n[UDP RX]", prxBuf->phead, recLen);
    buf = prxBuf->phead;
    bytesOfLen = mqtt_num_rem_len_bytes(buf + 3);
     if(bytesOfLen<1 || bytesOfLen>4)
    {
         return;
    }
    dataLen = mqtt_parse_rem_len(buf + 3);

    /* head(4B) | len(xB) | flag(1B) | cmd(2B) | payload(xB) */
    offsetPayload = LAN_PROTOCOL_HEAD_LEN + bytesOfLen + LAN_PROTOCOL_FLAG_LEN + 
                            LAN_PROTOCOL_CMD_LEN;
    packetLen = LAN_PROTOCOL_HEAD_LEN + bytesOfLen + dataLen;
    
    if(packetLen != recLen)
    {
        GAgent_Printf(GAGENT_WARNING, "Invalid UDP packet length");
        return;
    }

    cmd = *(uint16 *)(buf + bytesOfLen + LAN_PROTOCOL_HEAD_LEN + LAN_PROTOCOL_FLAG_LEN);
    cmd = ntohs(cmd);
    switch(cmd)
    {
        case GAGENT_LAN_CMD_ONDISCOVER:
            resetPacket(prxBuf);
            LAN_onDiscoverAck(pgc, prxBuf->phead, paddr);
            break;
        case GAGENT_LAN_CMD_ONBOARDING:
        {
            uint16 TempWiFiStatus = 0;
            TempWiFiStatus = pgc->rtinfo.GAgentStatus;
            
            if( (TempWiFiStatus&WIFI_MODE_AP)!= WIFI_MODE_AP )
                return ;
            if( (TempWiFiStatus&WIFI_MODE_ONBOARDING)!= WIFI_MODE_ONBOARDING )
                return ;
            
            if(RET_SUCCESS != Lan_udpOnBoarding(pgc, prxBuf->phead + offsetPayload))
            {
                GAgent_Printf(GAGENT_ERROR, "Invalid wifi_ssid or wifi_key  length");
            }
            else
            {
                GAgent_Printf( GAGENT_DEBUG,"file:%s function:%s line:%d ",__FILE__,__FUNCTION__,__LINE__ );
                TempWiFiStatus = GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,0 );
                //GAgent_DRVWiFi_StationCustomModeStart(pgc->gc.wifi_ssid, pgc->gc.wifi_key, WIFI_MODE_STATION);
            }
            resetPacket(prxBuf);
            LAN_onBoardingAck(pgc, prxBuf->phead, paddr);
            
            break;
        }
        default:
            break;
    }
}

static int32 Lan_udpOnBoarding(pgcontext pgc, uint8 *buf)
{
    uint16 ssidlength;
    uint16 passwdlength;
    
    ssidlength = buf[1] | (buf[0] << 8);
    passwdlength = buf[2+ssidlength+1] | (buf[2+ssidlength] << 8);

    if(ssidlength > SSID_LEN_MAX || passwdlength > WIFIKEY_LEN_MAX)
    {
        GAgent_Printf(GAGENT_CRITICAL, "ssid(len:%d) or pwd(len:%d) invalid",
                        ssidlength, passwdlength);
        return RET_FAILED;
    }

    strncpy(pgc->gc.wifi_ssid, (char *)(buf+2), ssidlength);
    pgc->gc.wifi_ssid[ssidlength] = '\0';
    strncpy(pgc->gc.wifi_key, (char *)(buf+2+ssidlength+2), passwdlength);
    pgc->gc.wifi_key[passwdlength] = '\0';
    pgc->gc.flag |= XPG_CFG_FLAG_CONNECTED;
    pgc->gc.flag |= XPG_CFG_FLAG_CONFIG;
    GAgent_DevSaveConfigData(&(pgc->gc));

    return RET_SUCCESS;
}

/****************************************************************
        FunctionName        :   LAN_onDiscoverAck.
        Description         :   reponsing client discover ack.
        Add by Will.zhou     --2015-03-10
****************************************************************/
static void LAN_onDiscoverAck(pgcontext pgc, uint8 *ptxBuf, struct sockaddr_t *paddr)
{
    int32 len;
    int32 ret;

    combination_broadcast_packet( pgc,ptxBuf,GAGENT_LAN_CMD_REPLY_BROADCAST );

    len = ptxBuf[4] + LAN_PROTOCOL_HEAD_LEN + 1;
    GAGENT_PRINT_DATA(GAGENT_INFO, "\n[UDP TX discover ack]", ptxBuf, len);
    ret = Socket_sendto(pgc->ls.udpServerFd, ptxBuf, len, paddr, sizeof(struct sockaddr_t));
    if(ret != len)
    {
        GAgent_Printf(GAGENT_ERROR,"send discover response fail,len:%d.ret:0x%x",
                        len, ret);
    }
    else
    {
        GAgent_Printf( GAGENT_DEBUG,"GAgent ack app discover ret=%d...",ret);
    }
}

static void LAN_onBoardingAck(pgcontext pgc, uint8 *ptxBuf, struct sockaddr_t *paddr)

{
    int32 len;
    uint32 dataLen;
    uint32 offset;
    int32 ret;
    
    /*  head(4B) */
    offset = 0;
    *(uint32 *)ptxBuf = htonl(GAGENT_PROTOCOL_VERSION);
    offset += LAN_PROTOCOL_HEAD_LEN;
    
    /* datalen = flag(1B) + cmd(2B) */
    dataLen = LAN_PROTOCOL_FLAG_LEN + LAN_PROTOCOL_CMD_LEN;
    ptxBuf[offset] = dataLen;
    offset += 1;
    
    /* flag */
    ptxBuf[offset] = 0x00;
    offset += LAN_PROTOCOL_FLAG_LEN;
    
    /* cmd */
    *(uint16 *)(ptxBuf + offset) = htons(GAGENT_LAN_CMD_REPLY_ONBOARDING);
    offset += LAN_PROTOCOL_CMD_LEN;

    len = offset;
    GAGENT_PRINT_DATA(GAGENT_INFO, "\n[UDP TX configure ack]", ptxBuf, len);
    ret = Socket_sendto(pgc->ls.udpServerFd, ptxBuf, len, paddr, sizeof(struct sockaddr_t));
    if(ret != len)
    {
        GAgent_Printf(GAGENT_ERROR,"send onboarding response fail,len:%d.ret:0x%x",len, ret);
    }
}

