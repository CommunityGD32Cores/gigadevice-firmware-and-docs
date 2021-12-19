#ifndef _GAGENT_NET_H_
#define _GAGENT_NET_H_

#include "lwip/sockets.h"

#if LWIP_IPV6
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET6) ? ip6addr_ntoa_r((src),(dst),(size)) \
     : (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL))
#define inet_pton(af,src,dst) \
    (((af) == AF_INET6) ? inet6_aton((src),(dst)) \
     : (((af) == AF_INET) ? inet_aton((src),(dst)) : 0))
#else /* LWIP_IPV6 */
#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL)
#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? inet_aton((src),(dst)) : 0)
#endif /* LWIP_IPV6 */


int Socket_sendto(int sockfd, u8 *data, int len, void *addr, int addr_size);
int Socket_accept(int sockfd, void *addr, socklen_t *addr_size);
int Socket_recvfrom(int sockfd, u8 *buffer, int len, void *addr, socklen_t *addr_size);
uint32 GAgent_GetHostByName( int8 *domain, int8 *IPAddress );
//int32  GAgent_accept( int32 sockfd );
//int32  GAgent_listen( int32 sockfd, int32 backlog );
//uint32 GAgent_sendto( int32  sockfd,  const  void  *buf, int32 len,  int32 flags );
int32 GAgent_select(int32 nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds,int32 sec,int32 usec );
int Gagent_setsocketnonblock(int socketfd);
           
                
/****************************************************************
*   functionName    :   GAgent_connect
*   flag            :   1 block 
*                       0 no block 
*   return          :   0> successful socketid
                         other fail.
****************************************************************/
int32 GAgent_connect( int32 iSocketId, uint16 port,
                        int8 *ServerIpAddr,int8 flag);

int32 GAgent_CreateTcpServer(uint16 tcp_port);
int32 GAgent_CreateUDPServer(uint16 udp_port);
int32 GAgent_CreateUDPBroadCastServer(uint16 udpbroadcast_port, struct sockaddr_t *sockaddr);
int32 Http_ResGetFirmware(pgcontext pgc,int32 socketid);

#endif

