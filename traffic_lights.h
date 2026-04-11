#ifndef TRAFFIC_LIGHTS_H
#define TRAFFIC_LIGHTS_H

typedef enum {
    GREEN_HORIZONTAL,
    GREEN_VERTICAL
} LightState;

typedef struct {
    int row;
    int col;
    LightState state;
    int timer;
} TrafficLight;

void init_traffic_lights(TrafficLight *lights, int *count,
                         char grid[][200], int rows, int cols);

void update_traffic_lights(TrafficLight *lights, int count, int cycle_time);

void print_traffic_lights(TrafficLight *lights, int count);

#endif
