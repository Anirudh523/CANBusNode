#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    int64_t last_hb_us;
    bool   hb_seen;
    uint32_t good_hb_streak;
    uint8_t peer_state;
    float throttle_pct;
    bool throttle_enable;
    float wheel_speed;
} app_state_t;

extern app_state_t     g_state;
extern SemaphoreHandle_t g_state_mutex;

void can_rx_task(void *arg);
void can_tx_task(void *arg);
void supervisor_task(void *arg);
