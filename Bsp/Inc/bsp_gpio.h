// BSP/Inc/bsp_gpio.h
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "board_config.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BSP_LED_0,      // PA5
    BSP_LED_NUM
} bsp_led_id_t;

int bsp_led_init(void);
void bsp_led_on(bsp_led_id_t id);
void bsp_led_off(bsp_led_id_t id);
void bsp_led_toggle(bsp_led_id_t id);

#endif