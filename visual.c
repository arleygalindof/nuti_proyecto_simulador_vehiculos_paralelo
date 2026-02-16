#include <stdio.h>
#include <unistd.h>

#include "grid.h"
#include "agents.h"

void render_simulation(char grid[MAX_ROWS][MAX_COLS],
                       int rows, int cols,
                       Vehicle *vehicles, int count)
{
    char buffer[MAX_ROWS][MAX_COLS];

    /*Copiar grid base*/
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            buffer[r][c] = grid[r][c];
        }
    }

    /*Dibujar vehículos en buffer*/
    for (int i = 0; i < count; i++) {

        char symbol;

        switch (vehicles[i].dir) {
            case UP:    symbol = '^'; break;
            case DOWN:  symbol = 'v'; break;
            case LEFT:  symbol = '<'; break;
            case RIGHT: symbol = '>'; break;
            default:    symbol = '?';
        }

        buffer[vehicles[i].row][vehicles[i].col] = symbol;
    }

    /*Limpiar pantalla*/
    printf("\033[H\033[J");

    /*Imprimir*/
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            char ch = buffer[r][c];
            int vehicle_index = -1;

            /* Buscar si hay vehículo en esa posición */
            for (int i = 0; i < count; i++) {
                if (vehicles[i].row == r &&
                    vehicles[i].col == c) {
                    vehicle_index = i;
                    break;
                }
            }

            if (vehicle_index != -1) {

                if (vehicles[vehicle_index].crashed) {
                    // 🔴 Vehículo estrellado → símbolo de dirección en rojo
                    printf("\033[31m%c\033[0m", ch);
                }
                else if (vehicles[vehicle_index].stopped) {
                    // 🟡 Vehículo detenido
                    printf("\033[33m%c\033[0m", ch);
                }
                else {
                    // 🟢 Vehículo en movimiento
                    printf("\033[32m%c\033[0m", ch);
                }
            }
            else {
                printf("%c", ch);
            }
        }
        printf("\n");
    }

    usleep(100000);  // 0.1 segundos
}
