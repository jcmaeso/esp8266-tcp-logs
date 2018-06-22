#include "tcp_logs.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "espconn.h"
#include "mem.h"

#define NET_DOMAIN "cn.bing.com"
#define pheadbuffer "GET / HTTP/1.1\r\nUser-Agent: curl/7.37.0\r\nHost: %s\r\nAccept: */*\r\n\r\n"
#define CONNECT_MSG "CONNECTED TO SERVER:\n"
#define packet_size   (2 * 1024)

int x;
LOCAL os_timer_t test_timer;
LOCAL struct espconn user_tcp_conn;
LOCAL struct _esp_tcp user_tcp;
ip_addr_t tcp_server_ip;

static tcp_log_params_t user_tcp_log_params[1];

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/

/******************************************************************************
 * FunctionName : user_tcp_recv_cb
 * Description  : receive callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
int string_compare(char str1[], char str2[])
{
    int ctr=0;

    while(str1[ctr]==str2[ctr])
    {
        if(str1[ctr]=='\0'||str2[ctr]=='\0')
            return 0;
        ctr++;
    }
    return 1;
}

LOCAL void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
    char data[] = "CONNECTED TO SERVER";
   //received some data from tcp connection
   os_printf("Fichero diferente\n");
   if(string_compare("CONNECTED TO SERVER",pusrdata)== 0){
        os_printf("Connected \r\n");
        os_printf("%s \r\n", pusrdata);
   }else{
       os_printf("OTRO");
       os_printf("%s \r\n", pusrdata);
   }
   
}


/******************************************************************************
 * FunctionName : user_tcp_sent_cb
 * Description  : data sent callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg)
{
   //data sent successfully

    os_printf("tcp sent succeed NEW !!! \r\n");
}
/******************************************************************************
 * FunctionName : user_tcp_discon_cb
 * Description  : disconnect callback.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg)
{
   //tcp disconnect successfully
   
    os_printf("tcp disconnect succeed !!! \r\n");
}
/******************************************************************************
 * FunctionName : user_esp_platform_sent
 * Description  : Processing the application data and sending it to the host
 * Parameters   : pespconn -- the espconn used to connetion with the host
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR user_sent_data(struct espconn *pespconn)
{
    char *pbuf = (char *)os_zalloc(packet_size);

    os_sprintf(pbuf, "Hola soy un ESP");

   espconn_sent(pespconn, pbuf, os_strlen(pbuf));
   
   os_free(pbuf);

}

/******************************************************************************
 * FunctionName : user_tcp_connect_cb
 * Description  : A new incoming tcp connection has been connected.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;

    os_printf("connect succeed !!! \r\n");

    if(user_tcp_log_params->recv_cb)
        espconn_regist_recvcb(pespconn, user_tcp_log_params->recv_cb);
    else
        espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
    
    if(user_tcp_log_params->sent_cb)
        espconn_regist_sentcb(pespconn, user_tcp_log_params->sent_cb);
    else
        espconn_regist_sentcb(pespconn, user_tcp_sent_cb);
    
    if(user_tcp_log_params->disconnect_cb)
        espconn_regist_disconcb(pespconn, user_tcp_log_params->disconnect_cb);
    else
        espconn_regist_disconcb(pespconn, user_tcp_discon_cb);

   
    user_sent_data(pespconn);
}

/******************************************************************************
 * FunctionName : user_tcp_recon_cb
 * Description  : reconnect callback, error occured in TCP connection.
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg, sint8 err)
{
   //error occured , tcp connection broke. user can try to reconnect here.
   
    os_printf("reconnect callback, error code %d !!! \r\n",err);
}

#ifdef DNS_ENABLE
/******************************************************************************
 * FunctionName : user_dns_found
 * Description  : dns found callback
 * Parameters   : name -- pointer to the name that was looked up.
 *                ipaddr -- pointer to an ip_addr_t containing the IP address of
 *                the hostname, or NULL if the name could not be found (or on any
 *                other error).
 *                callback_arg -- a user-specified callback argument passed to
 *                dns_gethostbyname
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
    struct espconn *pespconn = (struct espconn *)arg;

    if (ipaddr == NULL)
   {
        os_printf("user_dns_found NULL \r\n");
        return;
    }

   //dns got ip
    os_printf("user_dns_found %d.%d.%d.%d \r\n",
            *((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),
            *((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));

    if (tcp_server_ip.addr == 0 && ipaddr->addr != 0)
   {
      // dns succeed, create tcp connection
        os_timer_disarm(&test_timer);
        tcp_server_ip.addr = ipaddr->addr;
        os_memcpy(pespconn->proto.tcp->remote_ip, &ipaddr->addr, 4); // remote ip of tcp server which get by dns

        pespconn->proto.tcp->remote_port = 80; // remote port of tcp server
      
        pespconn->proto.tcp->local_port = espconn_port(); //local port of ESP8266

        espconn_regist_connectcb(pespconn, user_tcp_connect_cb); // register connect callback
        espconn_regist_reconcb(pespconn, user_tcp_recon_cb); // register reconnect callback as error handler

        espconn_connect(pespconn); // tcp connect
    }
}

/******************************************************************************
 * FunctionName : user_esp_platform_dns_check_cb
 * Description  : 1s time callback to check dns found
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
user_dns_check_cb(void *arg)
{
    struct espconn *pespconn = arg;

    espconn_gethostbyname(pespconn, NET_DOMAIN, &tcp_server_ip, user_dns_found); // recall DNS function

    os_timer_arm(&test_timer, 1000, 0);
}
#endif
/******************************************************************************
 * FunctionName : user_check_ip
 * Description  : check whether get ip addr or not
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_check_ip(void)
{
    struct ip_info ipconfig;

   //disarm timer first
    os_timer_disarm(&test_timer);

   //get ip info of ESP8266 station
    wifi_get_ip_info(STATION_IF, &ipconfig);

    if (wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0)
   {
      os_printf("got ip !!! \r\n");

      // Connect to tcp server as NET_DOMAIN
      user_tcp_conn.proto.tcp = &user_tcp;
      user_tcp_conn.type = ESPCONN_TCP;
      user_tcp_conn.state = ESPCONN_NONE;
      
#ifdef DNS_ENABLE
      tcp_server_ip.addr = 0;
       espconn_gethostbyname(&user_tcp_conn, NET_DOMAIN, &tcp_server_ip, user_dns_found); // DNS function

       os_timer_setfn(&test_timer, (os_timer_func_t *)user_dns_check_cb, &user_tcp_conn);
       os_timer_arm(&test_timer, 1000, 0);
#else

       const uint8_t test[4] = {192, 168, 1, 113};

       os_memcpy(user_tcp_conn.proto.tcp->remote_ip, (uint8_t*)user_tcp_log_params->ip_addr, 4);

       user_tcp_conn.proto.tcp->remote_port = user_tcp_log_params->port;  // remote port
     
       user_tcp_conn.proto.tcp->local_port = espconn_port(); //local port of ESP8266
     
    if(user_tcp_log_params->connect_cb)
       espconn_regist_connectcb(&user_tcp_conn, user_tcp_log_params->connect_cb); // register connect callback
    else
        espconn_regist_connectcb(&user_tcp_conn, user_tcp_connect_cb); // register connect callback
    
    if(user_tcp_log_params->reconnect_cb)
       espconn_regist_reconcb(&user_tcp_conn, user_tcp_log_params->reconnect_cb); // register reconnect callback as error handler
    else
       espconn_regist_reconcb(&user_tcp_conn, user_tcp_recon_cb); // register reconnect callback as error handler

       espconn_connect(&user_tcp_conn);

#endif
    }
   else
   {
       
        if ((wifi_station_get_connect_status() == STATION_WRONG_PASSWORD ||
                wifi_station_get_connect_status() == STATION_NO_AP_FOUND ||
                wifi_station_get_connect_status() == STATION_CONNECT_FAIL))
        {
         os_printf("connect fail !!! \r\n");
        }
      else
      {
           //re-arm timer to check ip
            os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
            os_timer_arm(&test_timer, 100, 0);
        }
    }
}


/******************************************************************************
 * FunctionName : user_set_station_config
 * Description  : set the router info which ESP8266 station will connect to
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_set_station_config(char* ssid, char* password)
{
    //tmp
    user_tcp_log_params->ip_addr[0] = 192;
    user_tcp_log_params->ip_addr[1] = 168;
    user_tcp_log_params->ip_addr[2] = 1;
    user_tcp_log_params->ip_addr[3] = 113;

    user_tcp_log_params->port = 6060;

    user_tcp_log_params->connect_cb = NULL;
    user_tcp_log_params->disconnect_cb = NULL;
    user_tcp_log_params->reconnect_cb = NULL;
    user_tcp_log_params->recv_cb = NULL;
    user_tcp_log_params->sent_cb = NULL;


   // Wifi configuration
   char ssid[32] = "Orange-9936";
   char password[64] = "56744935";
   struct station_config stationConf;
   
   os_memset(stationConf.ssid, 0, 32);
   os_memset(stationConf.password, 0, 64);
   
   //need not mac address
   stationConf.bssid_set = 0;
   
   //Set ap settings
   os_memcpy(&stationConf.ssid, ssid, 32);
   os_memcpy(&stationConf.password, password, 64);
   wifi_station_set_config(&stationConf);

   //set a timer to check whether got ip from router succeed or not.
   os_timer_disarm(&test_timer);
    os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
    os_timer_arm(&test_timer, 100, 0);

}