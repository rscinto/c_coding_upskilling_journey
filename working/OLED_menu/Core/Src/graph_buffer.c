#include "graph_buffer.h"

void graph_buffer_init(Graph_Buffer_t *graph, float min_display, float max_display)
{
    for (uint16_t i = 0; i < GRAPH_HISTORY_LEN; i++)
    {
        graph->samples[i] = 0.0f;
        graph->valid[i] = false;
    }

    graph->head = 0;
    graph->count = 0;
    graph->min_display = min_display;
    graph->max_display = max_display;
}

void graph_buffer_push(Graph_Buffer_t *graph, float value, bool value_valid)
{
    graph->samples[graph->head] = value;
    graph->valid[graph->head] = value_valid;

    graph->head = (graph->head + 1) % GRAPH_HISTORY_LEN;

    if (graph->count < GRAPH_HISTORY_LEN)
    {
        graph->count++;
    }
}

uint16_t graph_buffer_get_ordered_index(const Graph_Buffer_t *graph, uint16_t visible_x)
{
    uint16_t oldest;

    if (graph->count < GRAPH_HISTORY_LEN)
    {
        oldest = 0;
    }
    else
    {
        oldest = graph->head;
    }

    return (oldest + visible_x) % GRAPH_HISTORY_LEN;
}
