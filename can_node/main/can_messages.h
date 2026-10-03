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