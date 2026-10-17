/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/twai.h"


app_state_t g_state = {0};
SemaphoreHandle_t g_state_mutex;
void app_main(void)
{
    twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
    twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    ESP_ERROR_CHECK(twai_driver_install(&g, &t, &f));
    ESP_ERROR_CHECK(twai_start());

    uint8_t counter = 0;
    g_state_mutex = xSemaphoreCreateMutex();
    while (1) {
        twai_message_t tx = {.identifier = 0x100, .data_length_code = 1, .data = {counter++},};
        twai_transmit(&tx, pdMS_TO_TICKS(50));

        twai_message_t rx;
        if(twai_receive(&rx, pdMS_TO_TICKS(100)) == ESP_OK)
        {
            printf("ID 0x%031x len %d data0 %d\n", rx.identifier, rx.data_length_code, rx.data[0]);
        }
    }
}
