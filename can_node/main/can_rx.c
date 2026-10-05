#include "can_messages.h"
#include "app_state.h"
#define PEER_HEARTBEAT_ID (NODE_IS_COMMAND ? ID_ACT_HEARTBEAT : ID_CMD_HEARTBEAT)

void can_rx_task(void *arg){
    twai_message_t rx;
    while(1){
        if(twai_receive(&rx, portMAX_DELAY) != ESP_OK) continue;
        xSemaphoreTake(g_state_mutex, portMAX_DELAY);
        switch(rx.identifier) {
            case WHEEL_SPEED:
                g_state.wheel_speed = unpack_wheel_speed(&rx);
                break;
            case PEER_HEARTBEAT_ID:
                uint8_t counter, status;
                unpack_heartbeat(&rx, &counter, &status);
                g_state.last_hb_us = esp_timer_get_time();
                g_state.hb_seen = true;
                g_state.good_hb_streak++;
                g_state.peer_state = status;
                break;
            case THROTTLE_CMD:
                float pct;
                bool enable;
                unpack_throttle_cmd(&rx, &pct, &enable);
                g_state.throttle_pct = pct;
                g_state.throttle_enable = enable;
                break;
            default:
                break;
        }
        xSemaphoreGive(g_state_mutex);
    }
}