// BSP/Inc/bsp_timer.h
#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*bsp_timer_cb_t)(void);

int  bsp_timer_init(void);
bool bsp_timer_register_ms(uint32_t period_ms, bsp_timer_cb_t cb);

#endif