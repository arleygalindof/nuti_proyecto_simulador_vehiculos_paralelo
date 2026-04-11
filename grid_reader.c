#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 200
#define MAX_COLS 200

int is_valid_intersection(char grid[MAX_ROWS][MAX_COLS],
                          int r, int c, int rows, int cols) {

    int horizontal = 0;
    int vertical = 0;

    if (c > 0 && grid[r][c-1] == '.') horizontal = 1;
    if (c < cols-1 && grid[r][c+1] == '.') horizontal = 1;

    if (r > 0 && grid[r-1][c] == '.') vertical = 1;
    if (r < rows-1 && grid[r+1][c] == '.') vertical = 1;

    return horizontal && vertical;
}


int main() {
    FILE *file = fopen("grid.txt", "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char grid[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;
    char line[MAX_COLS];

    while (fgets(line, sizeof(line), file)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        if (rows == 0) cols = len;
        else if (len != cols) {
            printf("Error: la rejilla no es rectangular\n");
            return 1;
        }

        for (int i = 0; i < len; i++) {
            grid[rows][i] = line[i];
        }
        rows++;
    }
    fclose(file);

    printf("Intersecciones válidas encontradas:\n");

    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j] == '+') {
                if (is_valid_intersection(grid, i, j, rows, cols)) {
                    printf("  + en (%d, %d)\n", i, j);
                    count++;
                } else {
                    printf("  ⚠️ Intersección inválida en (%d, %d)\n", i, j);
                }
            }
        }
    }

    printf("\nTotal intersecciones válidas: %d\n", count);
    return 0;
}
