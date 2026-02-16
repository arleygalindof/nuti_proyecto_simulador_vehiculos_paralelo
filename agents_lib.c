#include "agents.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define PROB_CRASH_BORDER 0

/* --- Funciones Auxiliares --- */
static int is_cell_free(int r, int c, Vehicle *vehicles, int count) {
    for (int i = 0; i < count; i++) {
        if (!vehicles[i].crashed && vehicles[i].row == r && vehicles[i].col == c)
            return 0;
    }
    return 1;
}

static int find_vehicle_at(int r, int c, Vehicle *vehicles, int count) {
    for (int i = 0; i < count; i++) {
        if (!vehicles[i].crashed && vehicles[i].row == r && vehicles[i].col == c)
            return i;
    }
    return -1;
}

static int light_allows(TrafficLight *lights, int light_count, int r, int c, Direction dir) {
    for (int i = 0; i < light_count; i++) {
        if (lights[i].row == r && lights[i].col == c) {
            if ((dir == LEFT || dir == RIGHT) && lights[i].state == GREEN_HORIZONTAL) return 1;
            if ((dir == UP || dir == DOWN) && lights[i].state == GREEN_VERTICAL) return 1;
            return 0;
        }
    }
    return 1;
}

static int direction_valid(char grid[MAX_ROWS][MAX_COLS], int rows, int cols, int r, int c, Direction dir) {
    int nr = r, nc = c;
    if (dir == UP) nr--; else if (dir == DOWN) nr++;
    else if (dir == LEFT) nc--; else if (dir == RIGHT) nc++;
    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) return 0;
    return (grid[nr][nc] == '.' || grid[nr][nc] == '+');
}

static Direction opposite(Direction d) {
    switch (d) {
        case UP: return DOWN; case DOWN: return UP;
        case LEFT: return RIGHT; case RIGHT: return LEFT;
        default: return d;
    }
}

/* --- Inicialización Robusta --- */
void init_vehicles(Vehicle *vehicles, int count, char grid[MAX_ROWS][MAX_COLS], int rows, int cols) {
    srand(time(NULL));
    int real_rows = (rows > 0) ? rows : MAX_ROWS;
    int real_cols = (cols > 0) ? cols : MAX_COLS;
    int (*valid_cells)[2] = malloc(sizeof(int[2]) * (real_rows * real_cols));
    int total_valid = 0;

    for (int r = 0; r < real_rows; r++) {
        for (int c = 0; c < real_cols; c++) {
            if (grid[r][c] == '.' || grid[r][c] == '+') {
                valid_cells[total_valid][0] = r;
                valid_cells[total_valid][1] = c;
                total_valid++;
            }
        }
    }

    if (total_valid == 0 || count > total_valid) {
        fprintf(stderr, "Error: No hay celdas suficientes (%d) para %d agentes.\n", total_valid, count);
        exit(1);
    }

    for (int i = 0; i < count; i++) {
        int idx = rand() % total_valid;
        vehicles[i].row = valid_cells[idx][0];
        vehicles[i].col = valid_cells[idx][1];
        vehicles[i].dir = rand() % 4;
        vehicles[i].stopped = 0;
        vehicles[i].crashed = 0;
    }
    free(valid_cells);
}

/* --- Lógica Principal Paralela --- */
void update_vehicles(Vehicle *vehicles, int count, char grid[MAX_ROWS][MAX_COLS], 
                     TrafficLight *lights, int light_count, int rows, int cols, int n_threads) {
    
    int actual_threads = (n_threads > 0) ? n_threads : 1;

    #pragma omp parallel num_threads(actual_threads)
    {
        unsigned int seed = (unsigned int)(time(NULL) ^ omp_get_thread_num());

        #pragma omp for schedule(dynamic, 10)
        for (int i = 0; i < count; i++) {
            if (vehicles[i].crashed) continue;

            int nr = vehicles[i].row, nc = vehicles[i].col;

            if (vehicles[i].stopped) {
                int tr = nr, tc = nc;
                if (vehicles[i].dir == UP) tr--; else if (vehicles[i].dir == DOWN) tr++;
                else if (vehicles[i].dir == LEFT) tc--; else if (vehicles[i].dir == RIGHT) tc++;

                if (tr >= 0 && tr < rows && tc >= 0 && tc < cols && grid[tr][tc] != '#' && is_cell_free(tr, tc, vehicles, count)) {
                    vehicles[i].stopped = 0;
                }
            }

            if (!vehicles[i].stopped) {
                if (vehicles[i].dir == UP) nr--; else if (vehicles[i].dir == DOWN) nr++;
                else if (vehicles[i].dir == LEFT) nc--; else if (vehicles[i].dir == RIGHT) nc++;

                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || grid[nr][nc] == '#') {
                    vehicles[i].dir = opposite(vehicles[i].dir);
                    vehicles[i].stopped = 1;
                    continue;
                }

                if (grid[nr][nc] == '+' && !light_allows(lights, light_count, nr, nc, vehicles[i].dir)) {
                    vehicles[i].stopped = 1;
                    continue;
                }

                int other = find_vehicle_at(nr, nc, vehicles, count);
                if (other != -1 && other != i) {
                    #pragma omp critical(collision)
                    {
                        vehicles[i].crashed = 1; vehicles[other].crashed = 1;
                        vehicles[i].stopped = 1; vehicles[other].stopped = 1;
                    }
                } else {
                    vehicles[i].row = nr; vehicles[i].col = nc;
                    if (grid[nr][nc] == '+') {
                        Direction d = rand_r(&seed) % 4;
                        if (direction_valid(grid, rows, cols, nr, nc, d)) vehicles[i].dir = d;
                    }
                }
            }
        }
    }
}