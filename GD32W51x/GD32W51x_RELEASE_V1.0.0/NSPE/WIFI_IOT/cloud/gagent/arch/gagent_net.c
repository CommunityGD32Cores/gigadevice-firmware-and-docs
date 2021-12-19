#include "gagent_platform.h"
#include "gagent_net.h"
#include "gagent.h"
#include "gagent_md5.h"
#include "gagent_http.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"


int Socket_sendto(int sockfd, u8 *data, int len, void *addr, int addr_size)
{
    return sendto(sockfd, data, len, 0, (const struct sockaddr*)addr, addr_size);
}

int Socket_accept(int sockfd, void *addr, socklen_t *addr_size)
{
    return accept(sockfd, (struct sockaddr*)addr, addr_size);
}

int Socket_recvfrom(int sockfd, u8 *buffer, int len, void *addr, socklen_t *addr_size)
{
    return recvfrom(sockfd, buffer, len, 0, (struct sockaddr *)addr, addr_size);
}

//int connect_mqtt_socket(int iSocketId, struct sockaddr_t *Msocket_address, unsigned short port, char *MqttServerIpAddr)
//{
//    return 0;
//}

int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec )
{
    struct timeval t;
    
    t.tv_sec = sec;
    t.tv_usec = usec;
    return select( nfds,readfds,writefds,exceptfds,&t );
}

/****************************************************************
*   function    :   socket connect 
*   flag        :   1 block 
                    0 no block 
    return      :   0> connect ok socketid 
                :   other fail.
    add by Alex.lin  --2015-05-13
****************************************************************/
int32 GAgent_connect( int32 iSocketId, uint16 port,
                        int8 *ServerIpAddr,int8 flag)
{
    int8 ret=0;
    
    struct sockaddr_in Msocket_address;
    GAgent_Printf(GAGENT_INFO,"do connect ip:%s port=%d",ServerIpAddr,port );

    Msocket_address.sin_family = AF_INET;
    Msocket_address.sin_port= htons(port);
    Msocket_address.sin_addr.s_addr = inet_addr(ServerIpAddr);
/*
    unsigned long ul = 1;
    ioctl(iSocketId, FIONBIO, &ul);
*/
    ret = connect(iSocketId, (struct sockaddr *)&Msocket_address, sizeof( struct sockaddr_in));  
/*  
    if( 0==ret )
    {
        GAgent_Printf( GAGENT_INFO,"immediately connect ok !");
    }
    else
    {
        if( errno == EINPROGRESS )
        {
            int times = 0;
            fd_set rfds, wfds;
            struct timeval tv;
            int flags;
            tv.tv_sec = 10;   
            tv.tv_usec = 0;                 
            FD_ZERO(&rfds);  
            FD_ZERO(&wfds);  
            FD_SET(iSocketId, &rfds);  
            FD_SET(iSocketId, &wfds);    
            int selres = select(iSocketId + 1, &rfds, &wfds, NULL, &tv);
                switch( selres )
                {
                    case -1:
                        ret=-1;
                        break;
                    case 0:
                        ret = -1;
                        break;
                    default:
                        if (FD_ISSET(iSocketId, &rfds) || FD_ISSET(iSocketId, &wfds))
                        {
                            connect(iSocketId, (struct sockaddr *)&Msocket_address, sizeof( struct sockaddr_in));   
                            int err = errno;  
                            if  (err == EISCONN)  
                            {  
                                GAgent_Printf( GAGENT_INFO,"1 connect finished .\n");  
                                ret = 0;  
                            }
                            else
                            {
                                ret=-1;
                            } 
                            char buff[2];  
                            if (read(iSocketId, buff, 0) < 0)  
                            {  
                                GAgent_Printf( GAGENT_INFO,"connect failed. errno = %d\n", errno);  
                                ret = errno;  
                            }  
                            else  
                            {  
                                GAgent_Printf( GAGENT_INFO,"2 connect finished.\n");  
                                ret = 0;  
                            }  
                        }
                }
        }
    }
    //
    ioctl(iSocketId, FIONBIO, &ul);
    //
    */
    if( ret==0){
        GAgent_Printf(GAGENT_INFO,"Connect successfully. ");
        return iSocketId;
    }
    else 
    	return  -1;
}

uint32 GAgent_GetHostByName( int8 *domain, int8 *IPAddress)
{
	struct hostent *hptr;
    char   **pptr;
    char   str[32];

    memset(str, 0x0, sizeof(str));
    hptr = lwip_gethostbyname(domain);
    if (hptr == NULL)
    {
        GAgent_Printf(GAGENT_DEBUG," resean : \n");
        return 1;
    }
    pptr=hptr->h_addr_list;

    for(; *pptr!=NULL; pptr++)
    {
        inet_ntop(hptr->h_addrtype, (const ip_addr_t *)*pptr, str, sizeof(str));
    }
    inet_ntop(hptr->h_addrtype, (const ip_addr_t *)hptr->h_addr, str, sizeof(str));
    GAgent_Printf(GAGENT_INFO,"Server name %s  address: %s", domain, str);
    memcpy(IPAddress, str, 32);
    return 0;
}

int Gagent_setsocketnonblock(int socketfd)
{
	//DID NOT HAVE AN COMMMENT
#if 0
	int flags = fcntl(socketfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	return fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
#else
    return 0;
#endif
}

/****************************************************************
Function    :   GAgent_CreateTcpServer
Description :   creat TCP server.
tcp_port    :   server port.
return      :   0> the socket id .
                other error.
Add by Alex.lin     --2015-04-24.
****************************************************************/
int32 GAgent_CreateTcpServer(uint16 tcp_port)
{
    struct sockaddr_t addr;
    int32 bufferSize=0;
    int32 serversocketid=0;

    serversocketid = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( serversocketid < 0 )
    {
        serversocketid = INVALID_SOCKET;
        GAgent_Printf(GAGENT_ERROR, "TCPServer socket create error");
        return RET_FAILED;
    }
    bufferSize = SOCKET_TCPSOCKET_BUFFERSIZE;
    setsockopt( serversocketid, SOL_SOCKET, SO_RCVBUF, &bufferSize, 4 );
    setsockopt( serversocketid, SOL_SOCKET, SO_SNDBUF, &bufferSize, 4 );
    memset(&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(tcp_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if( bind( serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
        GAgent_Printf(GAGENT_ERROR, "TCPSrever socket bind error");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    if(listen( serversocketid, LAN_TCPCLIENT_MAX ) != 0 )
    {
        GAgent_Printf( GAGENT_ERROR, "TCPServer socket listen error!");
        close( serversocketid );
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
    return serversocketid;
}

int32 GAgent_CreateUDPServer(uint16 udp_port)
{
    int32 serversocketid = 0;
    struct sockaddr_t addr;
    
    serversocketid = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(serversocketid < 0)
    {
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket create error");
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
    
    if(Gagent_setsocketnonblock(serversocketid) != 0)
    {
        GAgent_Printf(GAGENT_ERROR,"UDP Server Gagent_setsocketnonblock fail.");
    }
    memset(&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(udp_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if( bind( serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
        GAgent_Printf(GAGENT_ERROR, "UDPServer socket bind error");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    GAgent_Printf(GAGENT_DEBUG,"UDP Server socketid:%d on port:%d", serversocketid, udp_port);
    return serversocketid;
}

int32 GAgent_CreateUDPBroadCastServer(uint16 udpbroadcast_port, struct sockaddr_t *sockaddr)
{
    int udpbufsize=2;
    int32 serversocketid = 0;
 /*   struct sockaddr_t addr;
    memset( &addr, 0, sizeof(addr) );*/

    serversocketid = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(serversocketid < 0) 
    {
        GAgent_Printf(GAGENT_DEBUG, "UDP BC socket create error");
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    if(Gagent_setsocketnonblock(serversocketid) != 0)
    {
        GAgent_Printf(GAGENT_DEBUG,"UDP BC Server Gagent_setsocketnonblock fail.");
    }

/*    addr.sin_family = AF_INET;
    addr.sin_port = htons(udpbroadcast_port);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);*/
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_port = htons(udpbroadcast_port);
    sockaddr->sin_addr.s_addr = htonl(INADDR_BROADCAST);

    if( setsockopt(serversocketid, SOL_SOCKET, SO_BROADCAST, &udpbufsize,sizeof(int)) != 0 )
    {
        GAgent_Printf(GAGENT_DEBUG,"UDP BC Server setsockopt error!");
    }
/*    if(bind(serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        GAgent_Printf(GAGENT_DEBUG,"UDP BC Server bind error!");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
    }*/
    GAgent_Printf(GAGENT_DEBUG,"UDP BC Server socketid:%d on port:%d", serversocketid, udpbroadcast_port);
    //*sockaddr = addr;
    return serversocketid;
}

extern uint32 newImg2Addr;
extern uint32 oldImg2Addr;
extern uint32 userDataAddr;
extern OTATYPE otaType;

int32 Http_ResGetFirmware(pgcontext pgc,int32 socketid)
{
    int ret;
    uint8 *httpReceiveBuf = NULL;
    int headlen = 0;
    uint8 MD5[16] = {0};
    uint8 md5_calc[16] = {0};
    int offset = 0;
    uint8 *buf = NULL;
    int writelen = 0;
    MD5_CTX ctx;

    httpReceiveBuf = malloc(SOCKET_RECBUFFER_LEN);
    if(httpReceiveBuf == NULL)
    {
        GAgent_Printf(GAGENT_INFO, "[CLOUD]%s malloc fail!len:%d", __func__, SOCKET_RECBUFFER_LEN);
        return RET_FAILED;
    }

    ret = Http_ReadSocket( socketid, httpReceiveBuf, SOCKET_RECBUFFER_LEN );  
    if(ret <=0 ) 
    { 
        free(httpReceiveBuf);
        return RET_FAILED;
    }
    
    ret = Http_Response_Code( httpReceiveBuf );
    if(200 != ret)
    {
        free(httpReceiveBuf);
        return RET_FAILED;
    }
    headlen = Http_HeadLen( httpReceiveBuf );
    pgc->rtinfo.filelen = Http_BodyLen( httpReceiveBuf );
//    pgc->mcu.MD5 = (char *)malloc(32+1);
//    if( pgc->mcu.MD5 == NULL )
//    {
//        return RET_FAILED;
//    }
    Http_GetMD5( httpReceiveBuf,MD5,pgc->mcu.MD5);
    Http_GetSV( httpReceiveBuf,(char *)pgc->mcu.soft_ver);
  
    offset = 0;
    buf = httpReceiveBuf + headlen;
    writelen = SOCKET_RECBUFFER_LEN - headlen;
    GAgent_MD5Init(&ctx);

	if( OTATYPE_WIFI == otaType ){
		if(newImg2Addr == 0xFFFFFFFF){
			GAgent_Printf(GAGENT_WARNING, "\n\r[%s] illegal ota addr 0x%x", __FUNCTION__, newImg2Addr);
			return RET_FAILED;			
		}		
		if( oldImg2Addr != 0xFFFFFFFF && newImg2Addr < oldImg2Addr ){
			if( pgc->rtinfo.filelen > (oldImg2Addr - newImg2Addr) ){	// firmware size too large
				GAgent_Printf(GAGENT_WARNING, "\n\r[%s] Part1 size < OTA size", __FUNCTION__);
				return RET_FAILED;
			}
		}else if(pgc->rtinfo.filelen > (userDataAddr - newImg2Addr)){
			GAgent_Printf(GAGENT_WARNING, "\n\r[%s] Part2 size < OTA size", __FUNCTION__);
			return RET_FAILED;
		}
		//erase sector
		GAgent_DeleteFile( newImg2Addr, pgc->rtinfo.filelen );
	}else{
		if(userDataAddr & 0xfff != 0){
			GAgent_Printf(GAGENT_WARNING, "\n\r[%s] illegal user data addr 0x%x", __FUNCTION__, userDataAddr);
			return RET_FAILED;			
		}
		if( pgc->rtinfo.filelen > (1024*1024 - userDataAddr) ){	// firmware size too large
			GAgent_Printf(GAGENT_WARNING, "\n\r[%s] user data size < MCU OTA size", __FUNCTION__);
			return RET_FAILED;
		}
		GAgent_DeleteFile( userDataAddr, pgc->rtinfo.filelen );
	}
	
    do
    {
        ret = GAgent_SaveUpgradFirmware( offset, buf, writelen );
        if(ret < 0)
        {
            GAgent_Printf(GAGENT_INFO, "[CLOUD]%s OTA upgrad fail at off:0x%x", __func__, offset);
            free(httpReceiveBuf);
            return RET_FAILED;
        }
        offset += writelen;
        GAgent_MD5Update(&ctx, buf, writelen);
        writelen = pgc->rtinfo.filelen - offset;
        if(0 == writelen)
            break;
        if(writelen > SOCKET_RECBUFFER_LEN)
        {
            writelen = SOCKET_RECBUFFER_LEN;
        }
        writelen = Http_ReadSocket( socketid, httpReceiveBuf, writelen );    
        if(writelen <= 0 )
        {
            GAgent_Printf(GAGENT_INFO,"[CLOUD]%s, socket recv ota file fail!recived:0x%x", __func__, offset);
            free(httpReceiveBuf);
            return RET_FAILED;
        }
        buf = httpReceiveBuf;
    }while(offset < pgc->rtinfo.filelen);
    GAgent_MD5Final(&ctx, md5_calc);
    if(memcmp(MD5, md5_calc, 16) != 0)
    {
        GAgent_Printf(GAGENT_WARNING,"[CLOUD]md5 fail!");
        free(httpReceiveBuf);
        return RET_FAILED;
    }
    free(httpReceiveBuf);
    return RET_SUCCESS;
}

