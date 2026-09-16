// BSP/Src/bsp_gpio.c
#include "bsp_gpio.h"

int bsp_led_init(void) {
    LED_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
    return 0;
}

void bsp_led_on(bsp_led_id_t id) {
    if (id == BSP_LED_0) HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
}

void bsp_led_off(bsp_led_id_t id) {
    if (id == BSP_LED_0) HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
}

void bsp_led_toggle(bsp_led_id_t id) {
    if (id == BSP_LED_0) HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
}