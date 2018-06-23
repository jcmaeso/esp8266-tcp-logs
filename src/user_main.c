#include "osapi.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "espconn.h"
#include "mem.h"
#include "tcp_logs.h"
#include "stdio.h"

int counter = 0;

os_timer_t send_data_timer;

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
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}

void send_data(){
    char testString[10000];
    os_sprintf(testString,"test %d",counter);
    counter++;
    os_printf("%d",counter);
    log_sent_data(testString);
}

void set_cyclinc_data(){
   os_timer_disarm(&send_data_timer);
   os_timer_setfn(&send_data_timer, (os_timer_func_t *)send_data, NULL);
   os_timer_arm(&send_data_timer, 1000, 1);
}



/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{

    UART_SetBaudrate(UART0,BIT_RATE_74880);

    os_printf("SDK version:%s\n", system_get_sdk_version());
   
   //Set softAP + station mode
   wifi_set_opmode(STATIONAP_MODE);
   char ssid[32] = "Orange-9936";
   char password[64] = "56744935";
   //Server Definitions
   tcp_log_set_ip(192,168,1,113);
   tcp_log_set_port(6060);
   tcp_log_set_on_connect_cb(set_cyclinc_data);
   //ESP8266 connect to router
   user_set_station_config(ssid,password);


}