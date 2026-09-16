// Application/Src/app.c
#include "app.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_timer.h"
#include "bsp_tb6612.h"

static void led_blink_task(void) {
    bsp_led_toggle(BSP_LED_0);
    bsp_uart_printf(BSP_UART_1, "LED toggled, tick: %lu ms\r\n", bsp_get_tick_ms());
}

void App_Init(void) {
    /* 外设初始化 */
    bsp_led_init();
    bsp_uart_init(BSP_UART_1, 115200);
    bsp_timer_init();
    bsp_motor_init();

    /* 启动信息 */
    bsp_uart_printf(BSP_UART_1, "\r\n=== STM32F103ZET6 Boot ===\r\n");
    bsp_uart_printf(BSP_UART_1, "Board: %s\r\n", BOARD_NAME);

    /* 注册 LED 任务 */
    bsp_timer_register_ms(500, led_blink_task);

    /* 启动电机自检：每步 1500ms，占空比 500（50%） */
    bsp_motor_selftest_start(1500, 500);
    bsp_uart_printf(BSP_UART_1, "[APP] Motor selftest started\r\n");
}

void App_Loop(void) {
    /* 电机自检任务（非阻塞，主循环调用） */
    bsp_motor_selftest_task();

    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}