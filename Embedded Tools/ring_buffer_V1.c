#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 8


/**
 * @brief Fixed-size ring buffer object.
 *
 * Designed for embedded systems with no dynamic allocation.
 *
 * @note Not thread safe.
 */
typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    size_t head;
    size_t tail;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb->head == rb->tail;
}

bool ring_buffer_is_full(const ring_buffer_t *rb) {
    return (rb->head + 1) % BUFFER_SIZE == rb->tail;
}

/**
 * @brief Pushes a byte into the ring buffer.
 *
 * @param rb Pointer to ring buffer object.
 * @param data Byte to insert.
 *
 * @return true if successful.
 * @return false if buffer is full.
 *
 * @pre rb != NULL
 * @post Head index advances by one on success.
 */
bool ring_buffer_push(ring_buffer_t *rb, uint8_t data) {
    if ((rb->head + 1) % BUFFER_SIZE == rb->tail) {
            return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % BUFFER_SIZE;
    return true;
}

/**
 * @brief Pops a byte from the ring buffer.
 *
 * @param rb Pointer to ring buffer object.
 * @param data Byte to pop.
 *
 * @return true if successful.
 * @return false if buffer is empty.
 *
 * @pre rb != NULL
 * @post Tail index advances by one on success.
 */
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data) {
    // full case?
    //7 items in the buffer.
    //head could be
    //removing one more item would make the tail crash into the head, so we return false;
    if (rb->tail == rb->head) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;
    return true;

}

int main(void) {

    char data_set_one[] = {'A', 'B', 'C', 'D', 'E'};
    char data_set_two[] = {'F', 'G', 'H', 'I'};

    ring_buffer_t my_buffer;
    ring_buffer_init(&my_buffer);

    for (size_t i = 0; i <= 4; i++) {
        ring_buffer_push(&my_buffer, data_set_one[i]);
    }

    for (size_t i = 0; i<=1; i++) {
        uint8_t temp = 0;
        ring_buffer_pop(&my_buffer, &temp);
        printf("%c", temp);
    }

    printf("\n");

    for (size_t i = 0; i <= 3; i++) {
        ring_buffer_push(&my_buffer, data_set_two[i]);
    }

    for (size_t i = 0; i<=7; i++) {
        uint8_t temp = 0;
        ring_buffer_pop(&my_buffer, &temp);
        printf("%c", temp);
    }

    return 0;
}