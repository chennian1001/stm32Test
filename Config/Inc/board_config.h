// Config/Inc/board_config.h
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32f1xx_hal.h"  // 唯一允许包含HAL的地方

/* ========== 板级特性开关 ========== */
#define BOARD_NAME          "STM32F103ZET6_DevBoard"
#define BOARD_HAS_LED       1
#define BOARD_HAS_USART1    1
#define BOARD_HAS_SOFT_TIMER 1

/* ========== 引脚定义 ========== */
#define LED_GPIO_PORT       GPIOA
#define LED_GPIO_PIN        GPIO_PIN_5
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

/* ========== 外设句柄外部声明 ========== */
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

#endif // BOARD_CONFIG_H