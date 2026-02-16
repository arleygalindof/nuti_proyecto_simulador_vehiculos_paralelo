#ifndef GRID_H
#define GRID_H

#define MAX_ROWS 200
#define MAX_COLS 200

int load_grid(const char *filename,
              char grid[MAX_ROWS][MAX_COLS],
              int *rows, int *cols);

void print_grid(char grid[MAX_ROWS][MAX_COLS],
                int rows, int cols);

#endif
