#include <stdio.h>

#define ROWS 100
#define COLS 100

int main() {

    FILE *f = fopen("grid_large.txt", "w");

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {

            if (r % 4 == 0 && c % 6 == 0)
                fputc('+', f);  // intersección
            else if (r % 4 == 0)
                fputc('.', f);  // vía horizontal
            else if (c % 6 == 0)
                fputc('.', f);  // vía vertical
            else
                fputc('#', f);  // bloque
        }
        fputc('\n', f);
    }

    fclose(f);
    printf("Grid generado: %dx%d\n", ROWS, COLS);

    return 0;
}
