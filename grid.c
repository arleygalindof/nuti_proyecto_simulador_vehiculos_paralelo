#include "grid.h"
#include <stdio.h>
#include <string.h>

int load_grid(const char *filename,
              char grid[MAX_ROWS][MAX_COLS],
              int *rows, int *cols) {

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir grid");
        return 0;
    }

    char line[MAX_COLS];
    *rows = 0;

    while (fgets(line, sizeof(line), file)) {
        int len = strlen(line);

        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        /*if (*rows == 0) {
            *cols = len;
        } else if (len != *cols) {
            printf("Error: rejilla no rectangular\n");
            return 0;
        }*/

        for (int i = 0; i < len; i++) {
            char c = line[i];
            if (c != '.' && c != '+' && c != '#') {
                printf("Error: caracter inválido %c\n", c);
                return 0;
            }
            grid[*rows][i] = c;
        }

        (*rows)++;
    }

    fclose(file);
    return 1;
}

void print_grid(char grid[MAX_ROWS][MAX_COLS],
                int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            putchar(grid[i][j]);
        }
        putchar('\n');
    }
}
