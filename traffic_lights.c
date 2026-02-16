#include "traffic_lights.h"
#include <stdio.h>

void init_traffic_lights(TrafficLight *lights, int *count,
                         char grid[][200], int rows, int cols) {
    *count = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == '+') {
                lights[*count].row = i;
                lights[*count].col = j;
                lights[*count].state = GREEN_HORIZONTAL;
                lights[*count].timer = 0;
                (*count)++;
            }
        }
    }
}

void update_traffic_lights(TrafficLight *lights, int count, int cycle_time) {
    for (int i = 0; i < count; i++) {
        lights[i].timer++;

        if (lights[i].timer >= cycle_time) {
            lights[i].timer = 0;
            lights[i].state =
                (lights[i].state == GREEN_HORIZONTAL)
                ? GREEN_VERTICAL
                : GREEN_HORIZONTAL;
        }
    }
}

void print_traffic_lights(TrafficLight *lights, int count) {
    for (int i = 0; i < count; i++) {
        printf("Semaforo (%d,%d): %s\n",
               lights[i].row,
               lights[i].col,
               lights[i].state == GREEN_HORIZONTAL
                   ? "HORIZONTAL"
                   : "VERTICAL");
    }
}
