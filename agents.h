#ifndef AGENTS_H
#define AGENTS_H

#include "grid.h"
#include "traffic_lights.h"

typedef enum {
    UP, DOWN, LEFT, RIGHT
} Direction;

typedef struct {
    int row;          // fila actual
    int col;          // columna actual
    Direction dir;    // dirección actual (UP, DOWN, LEFT, RIGHT)
    int stopped;      // 1 si está detenido, 0 si está en movimiento
    int crashed;      // 1 si está estrellado
    int turned_around; // 1 si se detuvo frente a un estrellado y giró
} Vehicle;

void init_vehicles(Vehicle *vehicles, int count,
                   char grid[MAX_ROWS][MAX_COLS],
                   int rows, int cols);

void update_vehicles(Vehicle *vehicles, int count,
                     char grid[MAX_ROWS][MAX_COLS],
                     TrafficLight *lights, int light_count,
                     int rows, int cols, int num_threads);

void print_vehicles(Vehicle *vehicles, int count);

void render_simulation(char grid[MAX_ROWS][MAX_COLS],
                       int rows, int cols,
                       Vehicle *vehicles, int count);

#endif
