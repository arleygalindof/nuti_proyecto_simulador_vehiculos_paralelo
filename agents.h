#ifndef AGENTS_H
#define AGENTS_H

#include "grid.h"
#include "traffic_lights.h"

typedef enum {
    UP, DOWN, LEFT, RIGHT
} Direction;

typedef struct {
    int row;
    int col;
    Direction dir;
    int stopped; // 🔥 ahora es DERIVADO, no controlador
} Vehicle;

void init_vehicles(Vehicle *vehicles, int count,
                   char grid[MAX_ROWS][MAX_COLS],
                   int rows, int cols);

void update_vehicles(Vehicle *vehicles, int count,
                     char grid[MAX_ROWS][MAX_COLS],
                     TrafficLight *lights, int light_count,
                     int rows, int cols);

void render_simulation(char grid[MAX_ROWS][MAX_COLS],
                       int rows, int cols,
                       Vehicle *vehicles, int count);

#endif