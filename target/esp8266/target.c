/*
 *  Copyright 2014-2016 CyberVision, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <lwip/mem.h>

#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>

#include "uart.h"

#define MAIN_STACK_SIZE 512

extern int main(void);

static void main_task(void *pvParameters);
static int wifi_init(void);
static int wifi_connect(const char *ssid, const char *pwd);
static void uart_init(void);

int target_initialize(void)
{

    if (wifi_init()) {
        printf("Error initialising wifi!\r\n");
        return 1;
    }

    if (wifi_connect(WIFI_SSID, WIFI_PASSWORD)) {
        printf("Couldn't connect to \"%s\" with password \"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
        return 1;
    }

    return 0;
}

void ICACHE_FLASH_ATTR user_init()
{
    uart_init();
    target_gpio_led_init();
    portBASE_TYPE error = xTaskCreate(main_task, (const signed char *)"main_task",
            MAIN_STACK_SIZE, NULL, 2, NULL );
    if (error < 0) {
        printf("Error creating main_task! Error code: %ld\r\n", error);
    }
}

static void main_task(void *pvParameters)
{
    (void)pvParameters;
    main();
    for (;;);
}


static void uart_init(void)
{
    uart_init_new();
    UART_SetPrintPort(UART0);
}

static int wifi_init(void)
{
    if (!wifi_set_opmode_current(0x01)) {
        return 1;
    }

    return 0;
}

static int wifi_connect(const char *ssid, const char *pwd)
{
    struct station_config sta_cfg;
    memset(&sta_cfg, 0, sizeof(sta_cfg));
    strcpy((char *)sta_cfg.ssid, ssid);
    strcpy((char *)sta_cfg.password, pwd);
    if (!wifi_station_set_config_current(&sta_cfg)) {
        return 1;
    }

    if (!wifi_station_connect()) {
        return 1;
    }

    uint8 status = STATION_IDLE;
    do {
        status = wifi_station_get_connect_status();
    } while (status == STATION_CONNECTING);

    switch (status) {
        case STATION_WRONG_PASSWORD:
        case STATION_NO_AP_FOUND:
        case STATION_CONNECT_FAIL: {
        	wifi_station_disconnect();
        	return 1;
        }
        case STATION_GOT_IP:
        case STATION_IDLE:
            break;
        default:
        	break;
    }

    return 0;
}

/* Required, don't touch */
void ets_putc(char c)
{
    os_putc(c);
}

int getchar(void)
{
    return uart_getchar();
}

