#include "gagent.h"
#include "lan.h"

#define webconfigHTML_HEAD_START \
"<html>\
<head>\
"

#define webconfig_onSubmitForm \
"<script>\
function onSubmitForm()\
{\
x=document.getElementById(\"Ssid\");\
y=document.getElementById(\"pwd\");\
if(x.value.length>32)\
{\
alert(\"SSID is too long!(1-32)\");\
return false;\
}\
if(y.value.length>32)\
{\
alert(\"Password length is between 8 to 32\");\
return false;\
}\
document.getElementById(\"submitForm\").submit()\
}\
</script>"

#define webconfig_HTML_BODY \
"</head>\
<body>\
<form action=\"web_config.cgi\" id=\"submitForm\"  method=\"post\">\
<div align=\"center\" style=\"font-size:30px; padding-top:100px;\">\
<p>[0~9],[a~z],[A~Z],[-],[_],[.]</p>\
<p>SSID:<input type=\"text\" name=\"ssid\" id=\"Ssid\" style=\"font-size:30px;\"/></p>\
<p>Pass:<input type=\"text\" name=\"pass\" id=\"pwd\" style=\"font-size:30px;\"/></p>\
<p><input type=\"button\" value=\"Submit\" onclick =onSubmitForm() style=\"font-size:30px;\"/></p>\
\r\n</form>" \
"</body>\r\n" \
"</html>"


#define webHTML_END \
"</form>" \
"</body>\r\n" \
"</html>"

 /* Generate the page index.html*/
static void GenerateHtmlPage(char* HeadPageBuffer, char* BodyPageBuffer, int32 length)
{
    strcpy( BodyPageBuffer, webconfigHTML_HEAD_START );
    strcat (BodyPageBuffer, webconfig_onSubmitForm);
    strcat (BodyPageBuffer, webconfig_HTML_BODY);
    printf("\r\n1. Generate Html Body Page, length: %d\n", strlen( BodyPageBuffer ));
    snprintf(HeadPageBuffer, length, "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html;charset=gb2312\r\n"
             "Content-Length: %d\r\n"
             "Cache-Control: no-cache\r\n"
             "Connection: close\r\n\r\n",
            (int)strlen(BodyPageBuffer));
    printf("\r\nGenerate Head Html Page, length: %d\n", strlen( HeadPageBuffer ));
}

 /* Generate the client ack page index.html*/
static void GenerateAckHtmlPage(char* HtmlPageBuffer, char* BodyPageBuffer, int32 length)
{
    snprintf(BodyPageBuffer, length, "%s", "<html><body>"
             "<div align=\"center\" style=\"font-size:40px; padding-top:100px;\">"
             "<p>Config the ssid and password successfully</p>"
             "</body></html>");
    printf("\r\n2. Generate ack Html Body Page, length: %d\n", strlen( BodyPageBuffer ));
    snprintf(HtmlPageBuffer, length, "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html;charset=gb2312\r\n"
             "Content-Length: %d\r\n"
             "Cache-Control: no-cache\r\n"
             "Connection: close\r\n\r\n",
             (int)strlen(BodyPageBuffer));
        printf("\r\nGenerate ack Head Html Page, length: %d\n", strlen( HtmlPageBuffer ));
}

int32 handleWebConfig( pgcontext pgc,int32 fd)
{
    int32   read_len;
    int8    *buf_head, *buf_body, *index_ssid, *index_pass, *p;
    pgconfig pConfigData=NULL;
    pConfigData = &(pgc->gc);

    if(fd <= 0)
    {
        return -1;
    }
    buf_head = (char *)malloc(1024);
    if (buf_head == NULL)
    {
        return -1;
    }
    memset(buf_head, 0, 1024);
    read_len = read(fd, buf_head, 1024);
    if(read_len <= 0)
    {
        free(buf_head);
        return -1;
    }
    buf_body = (char *)malloc(2048);

    if (buf_body == NULL)
    {
        free(buf_head);
        return -1;
    }
    memset(buf_body, 0, 2048);

    if( strstr(buf_head, "web_config.cgi") == NULL)
    {
        memset(buf_head, 0, 1024);
        GenerateHtmlPage(buf_head, buf_body, 1024);
        send(fd, buf_head, strlen(buf_head), 0);
        send(fd, buf_body, strlen(buf_body), 0);
    }
    else
    {
        /*post web_config.cgi*/
        index_ssid = strstr(buf_head, "ssid=");
        index_pass = strstr(buf_head, "pass=");
        if(index_ssid && index_pass)
        {
            index_ssid += strlen("ssid=");
            index_pass += strlen("pass=");
            p = strchr(index_ssid, '&');
            if(p) *p = '\0';
            p = strchr(index_pass, ' ');
            if(p) *p = '\0';

            if( !strncmp( buf_head, "GET", 3 ))
            {
                printf( "\n\r Get message from server");
                msleep(500);
                return 0;
            }
            else if (!strncmp( buf_head, "POST", 4 ) )
            {
                GAgent_Printf( GAGENT_CRITICAL,"web_config SSID and pass !" );
                memset(pConfigData->wifi_ssid, 0, SSID_LEN_MAX );
                memset(pConfigData->wifi_key, 0, WIFIKEY_LEN_MAX);

                memcpy(pConfigData->wifi_ssid, index_ssid, strlen(index_ssid));
                memcpy(pConfigData->wifi_key, index_pass, strlen(index_pass));
                pConfigData->wifi_ssid[ strlen(index_ssid) ] = '\0';
                pConfigData->wifi_key[ strlen(index_pass) ] = '\0';

                pConfigData->flag |= XPG_CFG_FLAG_CONNECTED;
                pConfigData->flag |= XPG_CFG_FLAG_CONFIG;
                pgc->ls.onboardingBroadCastTime = SEND_UDP_DATA_TIMES;
                GAgent_DevSaveConfigData( pConfigData );
                memset(buf_head, 0, 1024);
                GenerateAckHtmlPage(buf_head, buf_body, 1024);
                send(fd, buf_head, strlen(buf_head), 0);
                send(fd, buf_body, strlen(buf_body), 0);
                msleep(500);
                GAgent_DRVWiFi_APModeStop( pgc );
                GAgent_Printf( GAGENT_INFO,"webconfig ssid:%s key:%s",pConfigData->wifi_ssid,pConfigData->wifi_key );
                GAgent_Printf( GAGENT_DEBUG,"file:%s function:%s line:%d ",__FILE__,__FUNCTION__,__LINE__ );
                GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING,0 );
                GAgent_DevCheckWifiStatus( WIFI_MODE_STATION,1 );

           }
        }
    }
    free(buf_head);
    free(buf_body);
    msleep(100);
    return 0;
}
/****************************************************************
Function    :   Socket_CreateWebConfigServer
Description :   creat web config server.
tcp_port    :   web server port.#default 80
return      :   the web socket id .
Add by Alex.lin     --2015-04-24.
****************************************************************/
int32 GAgent_CreateWebConfigServer( uint16 tcp_port )
{
    return GAgent_CreateTcpServer( tcp_port );
}
/****************************************************************
Function    :   GAgent_DoTcpWebConfig
Description :   creat web config server.
return      :   NULL
Add by Alex.lin     --2015-04-25.
****************************************************************/
void GAgent_DoTcpWebConfig( pgcontext pgc )
{
    uint16 GAgentStatus=0;
    int32 newfd = 0;
    struct sockaddr_t addr;
    int32 addrLen= sizeof(addr);
    static uint32 StartOnboarding=1;

    GAgentStatus = pgc->rtinfo.GAgentStatus;

    if( (GAgentStatus&WIFI_MODE_AP)!= WIFI_MODE_AP )
    {
        if( pgc->ls.tcpWebConfigFd>0 )
        {
            close( pgc->ls.tcpWebConfigFd );
            pgc->ls.tcpWebConfigFd = INVALID_SOCKET;
        }
        return ;
    }
    if( (GAgentStatus&WIFI_MODE_ONBOARDING)!= WIFI_MODE_ONBOARDING )
        return ;

    if( pgc->ls.tcpWebConfigFd <= 0 )
    {
        GAgent_Printf( GAGENT_DEBUG,"Creat Tcp Web Server." );
        pgc->ls.tcpWebConfigFd = GAgent_CreateWebConfigServer( 80 );
    }

    if( pgc->ls.tcpWebConfigFd<0 )
    {
        return ;
    }
    if( !FD_ISSET(pgc->ls.tcpWebConfigFd, &(pgc->rtinfo.readfd)) )
    {
        return ;
    }

    newfd = Socket_accept(pgc->ls.tcpWebConfigFd, &addr, (socklen_t *)&addrLen);
    if(newfd < 0)
    {
        return ;
    }
    /*Reset onboarding time: GAGENT_ONBOARDING_TIME*/
    if (StartOnboarding)
    {
        GAgent_SetWiFiStatus( pgc, WIFI_MODE_ONBOARDING, 0 );
        GAgent_DevCheckWifiStatus( WIFI_MODE_ONBOARDING, 1);
        StartOnboarding = 0;
    }
    handleWebConfig( pgc,newfd);
    close(newfd);
    newfd = INVALID_SOCKET;
}



