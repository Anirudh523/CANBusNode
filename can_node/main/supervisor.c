typedef enum { ST_INIT, ST_OK, ST_DEGRADED, ST_FAULT} node_state_t;
static bool bus_off = false;

static void handle_bus_alerts(void){
    uint32_t alerts;
    if(twai_read_alerts(&alerts, 0) != ESP_OK) return;

    if (alerts & TWAI_ALERT_ERR_PASS) printf("bus: error passive\n");
    if (alerts & TWAI_ALERT_ERR_ACTIVE) printf("bus: error active again\n");
    if (alerts & TWAI_ALERT_RX_QUEUE_FULL) printf("bus: rx queue full, frames dropped\n");

    if(alerts & TWAI_ALERT_BUS_OFF){
        printf("bus: BUS OFF, starting recovery\n");
        bus_off = true;
        twai_initiate_recovery();
    }

    if(alerts & TWAI_ALERT_BUS_RECOVERED){
        printf("bus: recovered, restarting driver\n");
        bus_off = false;
        twai_start();
    }
}

static void log_bus_status(void){
    twai_status_info_t info;
    if(twai_get_status_info(&info) == ESP_OK) {
        printf("bus state=%d tex=%lu rec=%lu\n", info.state, (unsigned long)info.tx_error_counter,(insigned long)info.rx_error_counter);
    }
}

void supervisor_task(void *arg){
    node_state_t state = ST_INIT;
    while(1){
        handle_bus_alerts();
        int64_t age_ms = (esp_time_get_time() - g_state.last_hb_us) / 1000;
        node_state_t prev;
        prev = state;
        switch(state){
            case ST_INIT: if(g_state->hb_seen) state = ST_OK; break;
            case ST_OK: if(age_ms > 100) state = ST_DEGRADED; break;
            case ST_DEGRADED: if(age_ms > 300) state = ST_FAULT; break;
            case ST_FAULT: if(g_state->hb_seen) state = ST_OK; break;

        }
        bool late = age_ms > 150;
        if(late){
            xSemaphoreTake(g_state_mutex, portMAX_DELAY);
            g_state->good_hb_streak = 0;
        }
        apply_outputs(state);

        if(state != )
        printf("Previous state: %d now with heartbeat %d changing to state %d", prev, age_ms, state);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void inline apply_outputs(node_state_t *state){
    static float sim_speed = 0.0f;
    bool good_measurements = state == ST_OK || state == ST_DEGRADED;
    float effective = (good_measurements && g_state->throttle_enable) ? g_state->throttle_pct : 0.0f;
    sim_speed += (0.1*effective - sim_speed) * 0.1f;
    xSemaphoreTake(g_state_mutex, portMAX_DELAY);
    g_state->simulation_speed = sim_speed;
    g_state->my_state = state;
    xSemaphoreGive(g_state_mutex);
}