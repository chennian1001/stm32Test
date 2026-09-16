// BSP/Inc/bsp_uart.h
#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>

typedef enum {
    BSP_UART_1,
    BSP_UART_NUM
} bsp_uart_id_t;

#define BSP_UART_RX_BUF_SIZE  256

int  bsp_uart_init(bsp_uart_id_t id, uint32_t baudrate);
int  bsp_uart_send(bsp_uart_id_t id, const uint8_t *data, uint16_t len);
int  bsp_uart_printf(bsp_uart_id_t id, const char *fmt, ...);
void bsp_uart_set_rx_callback(bsp_uart_id_t id, void (*cb)(uint8_t *data, uint16_t len));

#endif