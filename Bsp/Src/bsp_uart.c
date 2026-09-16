// BSP/Src/bsp_uart.c
#include "bsp_uart.h"
#include "board_config.h"
#include "ring_buffer.h"   // Middleware层
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static ring_buffer_t rb_uart1;
static uint8_t uart1_rx_pool[BSP_UART_RX_BUF_SIZE];
static void (*uart1_rx_cb)(uint8_t *, uint16_t) = NULL;

int bsp_uart_init(bsp_uart_id_t id, uint32_t baudrate) {
    if (id != BSP_UART_1) return -1;
    rb_init(&rb_uart1, uart1_rx_pool, BSP_UART_RX_BUF_SIZE);
    // HAL_UART_Init 已在 CubeMX 的 MX_USART1_UART_Init() 中完成
    // 这里只需开启中断接收
    HAL_UART_Receive_IT(&huart1, &uart1_rx_pool[0], 1);  // 启动首字节接收
    return 0;
}

int bsp_uart_send(bsp_uart_id_t id, const uint8_t *data, uint16_t len) {
    if (id != BSP_UART_1) return -1;
    return (HAL_UART_Transmit(&huart1, (uint8_t*)data, len, 1000) == HAL_OK) ? 0 : -1;
}

int bsp_uart_printf(bsp_uart_id_t id, const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (len > 0) bsp_uart_send(id, (uint8_t*)buf, len);
    return len;
}

void bsp_uart_set_rx_callback(bsp_uart_id_t id, void (*cb)(uint8_t *, uint16_t)) {
    if (id == BSP_UART_1) uart1_rx_cb = cb;
}

/* ====== HAL回调，放在这里，不在it.c里写逻辑 ====== */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        static uint8_t rx_byte;
        rb_push(&rb_uart1, rx_byte);  // 存入环形缓冲区
        if (uart1_rx_cb) uart1_rx_cb(&rx_byte, 1);
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);  // 重新启动接收
    }
}