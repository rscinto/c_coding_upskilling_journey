#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define BUFFER_SIZE 64

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

//global buffer
ring_buffer_t my_buffer;
pthread_mutex_t lock;
sem_t empty_sem;
sem_t full_sem;


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
    if (rb->tail == rb->head) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % BUFFER_SIZE;
    return true;
}


void* producer(void* arg)
{
    static int successful_pushes = 0;

    printf("Producer running\n");
    ring_buffer_t* buffer = arg;
    for (int i = 0; i < 1000; i++) {

        //thread safe
        sem_wait(&empty_sem);
        pthread_mutex_lock(&lock);

        if (ring_buffer_push(buffer, (uint8_t)i)) {
            successful_pushes++;
        }

        pthread_mutex_unlock(&lock);
        sem_post(&full_sem);
    }
    return &successful_pushes;
}

void* consumer(void* arg)
{
    static int successful_pops = 0;
    printf("Consumer running\n");

    ring_buffer_t* buffer = arg;

    uint8_t data = 0;

    for (int i = 0; i < 1000; i++) {
        sem_wait(&full_sem);
        pthread_mutex_lock(&lock);

        if (ring_buffer_pop(buffer, &data)) {
            successful_pops++;
        }

        pthread_mutex_unlock(&lock);
        sem_post(&empty_sem);
    }

    return &successful_pops;
}


int main(void) {

    ring_buffer_init(&my_buffer);

    //Thread safe inits
    pthread_mutex_init(&lock, NULL);
    sem_init(&empty_sem, 0, BUFFER_SIZE - 1);
    sem_init(&full_sem, 0, 0);

    pthread_t prod;
    pthread_t cons;

    void* producer_result;
    void* consumer_result;

    pthread_create(&prod, NULL, producer, &my_buffer);
    pthread_create(&cons, NULL, consumer, &my_buffer);

    pthread_join(prod, &producer_result);
    pthread_join(cons, &consumer_result);

    const int* push_success = producer_result;
    const int* pop_success = consumer_result;

    printf("Successful pushes: %d \n", *push_success);
    printf("Successful pops: %d \n", *pop_success);
    printf("Final Head: %llu, Final Tail: %llu \n", my_buffer.head, my_buffer.tail);

    pthread_mutex_destroy(&lock);
    sem_destroy(&empty_sem);
    sem_destroy(&full_sem);

    //pthread_exit(NULL);
    return 0;
}
