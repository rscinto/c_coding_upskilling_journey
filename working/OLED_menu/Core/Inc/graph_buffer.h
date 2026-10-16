#ifndef INC_GRAPH_BUFFER_H_
#define INC_GRAPH_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

#define GRAPH_HISTORY_LEN 128
#define GRAPH_SAMPLE_PERIOD_MS 1000

typedef struct
{
    float samples[GRAPH_HISTORY_LEN];
    bool valid[GRAPH_HISTORY_LEN];

    uint16_t head;
    uint16_t count;

    float min_display;
    float max_display;
} Graph_Buffer_t;

void graph_buffer_init(Graph_Buffer_t *graph, float min_display, float max_display);
void graph_buffer_push(Graph_Buffer_t *graph, float value, bool value_valid);
uint16_t graph_buffer_get_ordered_index(const Graph_Buffer_t *graph, uint16_t visible_x);

#endif
