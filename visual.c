#include <stdio.h>
#include "agents.h"
#include <unistd.h>

/* 🔥 DEFINIR ANTES DE USAR */
static char vehicle_char(Direction d) {
    if (d == UP) return '^';
    if (d == DOWN) return 'v';
    if (d == LEFT) return '<';
    return '>';
}

void render_simulation(char grid[MAX_ROWS][MAX_COLS],
                       int rows, int cols,
                       Vehicle *vehicles, int count) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            int printed = 0;

            for (int k = 0; k < count; k++) {
                if (vehicles[k].row == i && vehicles[k].col == j) {

                    char symbol = vehicle_char(vehicles[k].dir);

                    if (vehicles[k].stopped)
                        printf("\033[31m%c\033[0m", symbol);
                    else
                        printf("\033[32m%c\033[0m", symbol);

                    printed = 1;
                    break;
                }
            }

            if (!printed) {
                putchar(grid[i][j]);
            }
        }
        putchar('\n');
    }

    printf("\n");
	usleep(500000);  // 0.1 segundos
}