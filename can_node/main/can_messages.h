#define THROTTLE_CMD 0x050
#define CMD_HEARTBEAT 0x100
#define ACT_HEARTBEAT 0x101
#define WHEEL_SPEED 0x200

static inline void pack_wheel_speed(twai_message_t *m, float mps){
    uint16_t raw = (uint16_t)(mps / 0.01f + 0.5f);
    m->identifier = WHEEL_SPEED;
    m->data_length_code = 2;
    m->data[0] = raw & 0xFF;
    m->data[1] = raw >> 8;
}

static inline float unpack_wheel_speed(const twai_message_t *m){
    uint16_t raw = m->data[0] | (m->data[1] << 8);
    return raw * 0.01f;
}

static inline void pack_throttle_cmd(twai_message_t *m, float pct, bool enable){
    if(pct < 0) pct = 0;
    if(pct > 100) pct = 100;
    *m = (twai_message_t){0};
    m->identifier = ID_THROTTLE_CMD;
    m->data_length_code = 2;
    uint16_t raw = (uint16_t)(pct / 0.01f + 0.5f);
    m->data[0] = (uint8_t)(pct / 0.5f + 0.5f);
    m->data[1] = enable ? 1 : 0;
}

static inline void unpack_throttle_cmd(const twai_message_t *m, float *pct, bool *enable) {
    *pct = m->data[0] * 0.5f;
    *enable = m->data[1] & 0x01;
}

static inline void pack_heartbeat(twai_message_t *m, uint32_t id, uint8_t counter, uint8_t status) {
    *m = (twai_message_t) {0};
    m->identifier = id;
    m->data_length_code = 2;
    m->data[0] = counter;
    m->data[1] = status;
}

static inline void unpack_heartbeat(const twai_message_t *m, uint8_t *counter, uint8_t *status) {
    *counter = m->data[0];
    *status = m->data[1];
}
