// BSP/Src/bsp_timer.c
#include "bsp_timer.h"
#include "board_config.h"

#define MAX_SOFT_TIMERS  8

typedef struct {
    uint32_t period;
    uint32_t counter;
    bsp_timer_cb_t callback;
    bool active;
} soft_timer_t;

static soft_timer_t timers[MAX_SOFT_TIMERS];
static volatile uint32_t sys_tick_ms = 0;

int bsp_timer_init(void) {
    // TIM2 已在 CubeMX 中配置为1ms中断
    HAL_TIM_Base_Start_IT(&htim2);
    return 0;
}

bool bsp_timer_register_ms(uint32_t period_ms, bsp_timer_cb_t cb) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (!timers[i].active) {
            timers[i].period = period_ms;
            timers[i].counter = 0;
            timers[i].callback = cb;
            timers[i].active = true;
            return true;
        }
    }
    return false;
}

/* ====== 在TIM2中断中调度软定时器 ====== */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        sys_tick_ms++;
        for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
            if (timers[i].active) {
                timers[i].counter++;
                if (timers[i].counter >= timers[i].period) {
                    timers[i].counter = 0;
                    if (timers[i].callback) timers[i].callback();
                }
            }
        }
    }
}

uint32_t bsp_get_tick_ms(void) {
    return sys_tick_ms;
}