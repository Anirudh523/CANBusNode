#include "app_state.h"
#include "can_messages.h"
#if NODE_IS_COMMAND
  #define MY_HEARTBEAT_ID    ID_CMD_HEARTBEAT
  #define PEER_HEARTBEAT_ID  ID_ACT_HEARTBEAT
#else
  #define MY_HEARTBEAT_ID    ID_ACT_HEARTBEAT
  #define PEER_HEARTBEAT_ID  ID_CMD_HEARTBEAT
#endif
void can_tx_task(void *arg){
    TickType_t last = xTaskGetTickCount();
    uint32_t tick = 0;
    uint32_t hb_counter = 0;
    twai_message_t tx = {0};
    while(1){
        app_state_t state = get_snapshot();

        if(tick % 2 == 0){
            pack_throttle(&tx, state.throttle_pct, state.throttle_enable);
            twai_transmit(&tx, portMAX_DELAY);
        } else if(tick % 5 == 0){
            pack_wheel_speed(&tx, state.wheel_speed);
            twai_transmit(&tx, portMAX_DELAY);
        } else if(tick % 10 == 0){
            pack_heartbeat(&tx, MY_HEARTBEAT_ID, state.last_hb_us, state.peer_state);
            twai_transmit(&tx, portMAX_DELAY);
        }

        tick++;
        vTaskDelayUntil(&last, pdMS_TO_TICKS(10));
    }
}

app_state_t get_snapshot(){
    app_state_t snapshot;
    xSemaphoreTake(g_state_mutex, portMAX_DELAY);
    snapshot = g_state;
    xSemaphoreGive(g_state_mutex);
    return snapshot;
}