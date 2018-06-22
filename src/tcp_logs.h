#include "osapi.h"
/******************************************************************************
 * FunctionName : user_set_station_config
 * Description  : set the router info which ESP8266 station will connect to
 * Parameters   : none
 * Returns      : none
*******************************************************************************/


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



void ICACHE_FLASH_ATTR user_set_station_config(void);