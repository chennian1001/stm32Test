// Middleware/Inc/ring_buffer.h
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t head;
    volatile uint16_t tail;
} ring_buffer_t;

void rb_init(ring_buffer_t *rb, uint8_t *pool, uint16_t pool_size);
bool rb_push(ring_buffer_t *rb, uint8_t data);
bool rb_pop(ring_buffer_t *rb, uint8_t *data);
uint16_t rb_count(ring_buffer_t *rb);

#endif