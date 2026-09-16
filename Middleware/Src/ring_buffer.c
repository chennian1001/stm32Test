// Middleware/Src/ring_buffer.c
#include "ring_buffer.h"

void rb_init(ring_buffer_t *rb, uint8_t *pool, uint16_t pool_size) {
    rb->buffer = pool;
    rb->size = pool_size;
    rb->head = 0;
    rb->tail = 0;
}

bool rb_push(ring_buffer_t *rb, uint8_t data) {
    uint16_t next = (rb->head + 1) % rb->size;
    if (next == rb->tail) return false;  // 满
    rb->buffer[rb->head] = data;
    rb->head = next;
    return true;
}

bool rb_pop(ring_buffer_t *rb, uint8_t *data) {
    if (rb->head == rb->tail) return false;  // 空
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

uint16_t rb_count(ring_buffer_t *rb) {
    return (rb->head >= rb->tail) ? (rb->head - rb->tail) : (rb->size - rb->tail + rb->head);
}