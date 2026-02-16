#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 10000   // tamaño máximo de una línea
#define MAX_ROWS 10000   // número máximo de filas

int main() {
    FILE *in = fopen("grid_extra_large.txt", "r");
    FILE *out = fopen("grid_rectangular.txt", "w");
    if (!in || !out) {
        printf("Error abriendo archivos.\n");
        return 1;
    }

    char *grid[MAX_ROWS];   // arreglo de punteros a filas
    int row_count = 0;
    int max_len = 0;

    // 1. Leer todas las filas y calcular la longitud máxima
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, in)) {
        int len = strlen(buffer);
        if (buffer[len-1] == '\n') {
            buffer[len-1] = '\0'; // quitar salto de línea
            len--;
        }
        if (len > max_len) max_len = len;

        // reservar memoria exacta para la fila
        grid[row_count] = malloc(len + 1);
        strcpy(grid[row_count], buffer);
        row_count++;
    }

    // 2. Normalizar todas las filas
    for (int i = 0; i < row_count; i++) {
        int len = strlen(grid[i]);
        char *normalized = malloc(max_len + 1);

        // copiar contenido existente
        strcpy(normalized, grid[i]);

        // rellenar con espacios hasta max_len
        for (int j = len; j < max_len; j++) {
            normalized[j] = ' '; // puedes usar '.' o '#'
        }
        normalized[max_len] = '\0';

        fprintf(out, "%s\n", normalized);

        free(grid[i]);
        free(normalized);
    }

    fclose(in);
    fclose(out);

    printf("Rejilla normalizada guardada en grid_rectangular.txt\n");
    return 0;
}