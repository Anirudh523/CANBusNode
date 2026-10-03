#pragma once
#include "driver/gpio.h"

#define CAN_TX_GPIO GPIO_NUM_4 GPIO_NUM_4
#define CAN_RX_GPIO GPIO_NUM_5 GPIO_NUM_5

#define NODE_IS_COMMAND 1
#define MY_ID (NODE_IS_COMMAND ? 0x100 : 0x101);