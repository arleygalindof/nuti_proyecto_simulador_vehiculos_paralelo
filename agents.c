#include "agents.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>
/* ============================= */
/* Helpers */
/* ============================= */

static int direction_type(Direction d) {
    return (d == LEFT || d == RIGHT) ? 0 : 1;
}

Direction opposite_direction(Direction d) {
    switch (d) {
        case UP: return DOWN;
        case DOWN: return UP;
        case LEFT:  return RIGHT;
        case RIGHT:  return LEFT;
    }
    return d; // fallback
}

static void next_position(int r, int c, Direction d, int *nr, int *nc) {
    *nr = r;
    *nc = c;

    if (d == UP) (*nr)--;
    else if (d == DOWN) (*nr)++;
    else if (d == LEFT) (*nc)--;
    else if (d == RIGHT) (*nc)++;
}

static int light_allows(TrafficLight *lights, int light_count,
                        int r, int c, Direction dir) {

    for (int i = 0; i < light_count; i++) {
        if (lights[i].row == r && lights[i].col == c) {

            if ((dir == LEFT || dir == RIGHT) &&
                lights[i].state == GREEN_HORIZONTAL) return 1;

            if ((dir == UP || dir == DOWN) &&
                lights[i].state == GREEN_VERTICAL) return 1;

            return 0;
        }
    }

    return 1;
}

static int direction_valid(char grid[MAX_ROWS][MAX_COLS],
                           int rows, int cols,
                           int r, int c, Direction dir) {

    int nr, nc;
    next_position(r, c, dir, &nr, &nc);

    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) return 0;

    return (grid[nr][nc] == '.' || grid[nr][nc] == '+');
}

/* ============================= */
/* Inicializaci¨®n */
/* ============================= */

void init_vehicles(Vehicle *vehicles, int count,
                   char grid[MAX_ROWS][MAX_COLS],
                   int rows, int cols) {

    srand(time(NULL));

    int (*valid_cells)[2] = malloc(sizeof(int[2]) * rows * cols);
    int total = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r][c] == '.' || grid[r][c] == '+') {
                valid_cells[total][0] = r;
                valid_cells[total][1] = c;
                total++;
            }
        }
    }

    if (count > total) {
        fprintf(stderr, "Error: no hay suficientes celdas\n");
        exit(1);
    }

    for (int i = 0; i < count; i++) {
        int idx = rand() % total;

        vehicles[i].row = valid_cells[idx][0];
        vehicles[i].col = valid_cells[idx][1];

        Direction possible[4];
        int n = 0;

        for (int d = 0; d < 4; d++) {
            if (direction_valid(grid, rows, cols,
                                vehicles[i].row,
                                vehicles[i].col,
                                (Direction)d)) {
                possible[n++] = (Direction)d;
            }
        }

        vehicles[i].dir = (n > 0) ? possible[rand() % n] : rand() % 4;

        vehicles[i].stopped = 0;
    }

    free(valid_cells);
}

/* ============================= */
/* Update */
/* ============================= */

void update_vehicles(Vehicle *vehicles, int count,
                     char grid[MAX_ROWS][MAX_COLS],
                     TrafficLight *lights, int light_count,
                     int rows, int cols) {

    Vehicle *next = malloc(sizeof(Vehicle) * count);

    int occupancy[MAX_ROWS][MAX_COLS][2];
    memset(occupancy, 0, sizeof(occupancy));

/* ========================= */
/* FASE 1: DECISIÓN (PARALELA) */
/* ========================= */
#pragma omp parallel for
for (int i = 0; i < count; i++) {

    int r = vehicles[i].row;
    int c = vehicles[i].col;
    Direction d = vehicles[i].dir;

    int nr, nc;
    next_position(r, c, d, &nr, &nc);

    int can_move = 1;

    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols ||
        grid[nr][nc] == '#') {
        can_move = 0;
    }
    else if (grid[nr][nc] == '+' &&
             !light_allows(lights, light_count, nr, nc, d)) {
        can_move = 0;
    }

    if (can_move) {
        next[i].row = nr;
        next[i].col = nc;
        next[i].dir = d;
    }
    else {

        Direction options[4];
        int opt_count = 0;

        for (int nd = 0; nd < 4; nd++) {

            if (nd == d) continue;

            if (direction_valid(grid, rows, cols, r, c, (Direction)nd)) {

                int tr, tc;
                next_position(r, c, (Direction)nd, &tr, &tc);

                if (grid[tr][tc] == '+' &&
                    !light_allows(lights, light_count, tr, tc, (Direction)nd)) {
                    continue;
                }

                options[opt_count++] = (Direction)nd;
            }
        }

        if (opt_count > 0) {
            Direction chosen = options[rand() % opt_count];

            int tr, tc;
            next_position(r, c, chosen, &tr, &tc);

            next[i].row = tr;
            next[i].col = tc;
            next[i].dir = chosen;

        } else {
            next[i] = vehicles[i];
        }
    }
}

    /* ===== FASE 2 ===== */
    for (int i = 0; i < count; i++) {

        int r = next[i].row;
        int c = next[i].col;
        int type = direction_type(next[i].dir);

        if (occupancy[r][c][type] > 1) {
            if (rand() % 2 == 0) {
                next[i] = vehicles[i];
            }
        }
    }

    /* ===== FASE 3 (?? FIX GIRO EN U) ===== */
    for (int i = 0; i < count; i++) {

        int old_r = vehicles[i].row;
        int old_c = vehicles[i].col;

        vehicles[i] = next[i];

        vehicles[i].stopped = (old_r == vehicles[i].row &&
                               old_c == vehicles[i].col);

        int r = vehicles[i].row;
        int c = vehicles[i].col;

        if (grid[r][c] == '+') {

            Direction options[4];
            int opt_count = 0;

            for (int nd = 0; nd < 4; nd++) {

                // ?? evitar giro en U
                if ((Direction)nd == opposite_direction(vehicles[i].dir))
                    continue;

                if (direction_valid(grid, rows, cols, r, c, (Direction)nd)) {
                    options[opt_count++] = (Direction)nd;
                }
            }

            if (opt_count > 0) {
                vehicles[i].dir = options[rand() % opt_count];
            }
        }
    }

    free(next);
}