#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    ring_buffer_t my_buffer;
    ring_buffer_init(&my_buffer);

    char command[16];
    char value;

    while (scanf("%15s", command) == 1) {
        if (strcmp(command, "push") == 0) {
            scanf(" %c", &value);

            if (ring_buffer_push(&my_buffer, (uint8_t)value)) {
                printf("OK push %c\n", value);
            } else {
                printf("FAIL full\n");
            }
        }
        else if (strcmp(command, "pop") == 0) {
            uint8_t out;

            if (ring_buffer_pop(&my_buffer, &out)) {
                printf("OK pop %c\n", out);
            } else {
                printf("FAIL empty\n");
            }
        }
    }

    return 0;
}