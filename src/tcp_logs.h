#include "osapi.h"


typedef void (* tcp_connect_cb)(void *arg);
typedef void (* tcp_reconnect_cb)(void *arg, sint8 err);
typedef void (* tcp_recv_cb)(void *arg, char *pdata, unsigned short len);
typedef void (* tcp_sent_cb)(void *arg);
//typedef void (* dns_found_callback)(const char *name, ip_addr_t *ipaddr, void *callback_arg);

typedef struct dns_log_params{
    unsigned int dns_enable;
    //dns_found_callback dns_cb;
}dns_log_params_t;

typedef struct tcp_log_params{
    unsigned char ip_addr[4];
    unsigned int port;
    dns_log_params_t* dns;
    tcp_connect_cb connect_cb;
    tcp_connect_cb disconnect_cb;
    tcp_reconnect_cb reconnect_cb;
    tcp_recv_cb recv_cb;
    tcp_sent_cb sent_cb;
}tcp_log_params_t;


/******************************************************************************
 * FunctionName : user_set_station_config
 * Description  : set the router info which ESP8266 station will connect to and initlizizes connection
 * Parameters   : char* ssid -- Network SSID char* password Password
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR user_set_station_config(char* ssid, char* password);
/******************************************************************************
 * FunctionName : tcp_log_set_ip
 * Description  : Sets server IP
 * Parameters   : ip1.ip2.ip3.ip4
 * Returns      : none
*******************************************************************************/
void tcp_log_set_ip(unsigned char ip1,unsigned char ip2,unsigned char ip3,unsigned char ip4);
/******************************************************************************
 * FunctionName : tcp_log_set_port
 * Description  : Sets server port
 * Parameters   : ip1.ip2.ip3.ip4
 * Returns      : none
*******************************************************************************/
void tcp_log_set_port(unsigned int port);
/******************************************************************************
 * FunctionName : tcp_log_set_connect_cb
 * Description  : Setts the callback function for connect event
 * Parameters   : function pointer to new cb function 
 * Returns      : none
*******************************************************************************/
void tcp_log_set_connect_cb(tcp_connect_cb connect_cb);
/******************************************************************************
 * FunctionName : tcp_log_set_disconnect_cb
 * Description  : Setts the callback function for disconnect event
 * Parameters   : function pointer to new cb function
 * Returns      : none
*******************************************************************************/
void tcp_log_set_disconnect_cb(tcp_connect_cb disconnect_cb);
/******************************************************************************
 * FunctionName : tcp_log_set_reconnect_cb
 * Description  : Setts the callback function for reconnect event
 * Parameters   : function pointer to new cb function
 * Returns      : none
*******************************************************************************/
void tcp_log_set_reconnect_cb(tcp_reconnect_cb reconnect_cb);
/******************************************************************************
 * FunctionName : tcp_log_set_recv_cb
 * Description  : Setts the callback function for recv event (recived data)
 * Parameters   : function pointer to new cb function
 * Returns      : none
*******************************************************************************/
void tcp_log_set_recv_cb(tcp_recv_cb recv_cb);
/******************************************************************************
 * FunctionName : tcp_log_set_sent_cb
 * Description  : Setts the callback function for disconnect event (sent data)
 * Parameters   : function pointer to new cb function
 * Returns      : none
*******************************************************************************/
void tcp_log_set_sent_cb(tcp_sent_cb sent_cb);