#include "agents.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// --- PROTOTIPOS DE FUNCIÓN ---
static int find_vehicle_at(int r, int c, Vehicle *vehicles, int count);
static int light_allows(TrafficLight *lights, int light_count, int r, int c, Direction dir);
static Direction opposite(Direction d);
static int direction_valid(char grid[MAX_ROWS][MAX_COLS], int rows, int cols, int r, int c, Direction dir);

// Mutex para proteger la zona crítica de colisiones
pthread_mutex_t collision_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int start;
    int end;
    Vehicle *vehicles;
    int count;
    char (*grid)[MAX_COLS];
    TrafficLight *lights;
    int light_count;
    int rows;
    int cols;
} ThreadData;

/* ========================================================== */
/* 1. INICIALIZACIÓN (Resuelve el error de undefined reference)*/
/* ========================================================== */
void init_vehicles(Vehicle *vehicles, int count, char grid[MAX_ROWS][MAX_COLS], int rows, int cols) {
    srand(time(NULL));
    int total_valid = 0;
    
    // Usamos MAX_COLS y MAX_ROWS directamente si los parámetros llegan mal
    int real_rows = (rows <= 0) ? MAX_ROWS : rows;
    int real_cols = (cols <= 0) ? MAX_COLS : cols;

    // Diagnóstico para maestría: imprimimos las dimensiones que usaremos
    printf("DEBUG CORREGIDO: Buscando en matriz de %dx%d...\n", real_rows, real_cols);

    // IMPORTANTE: valid_pos debe ser lo suficientemente grande
    int (*valid_pos)[2] = malloc(sizeof(int[2]) * (MAX_ROWS * MAX_COLS));
    if (!valid_pos) {
        perror("Error malloc");
        exit(1);
    }

    for (int r = 0; r < real_rows; r++) {
        for (int c = 0; c < real_cols; c++) {
            // Verificamos qué hay en la celda
            char cell = grid[r][c];
            if (cell == '.' || cell == '+') {
                valid_pos[total_valid][0] = r;
                valid_pos[total_valid][1] = c;
                total_valid++;
            }
        }
    }

    if (total_valid == 0) {
        fprintf(stderr, "ERROR: Sigue sin encontrar celdas. Revise MAX_COLS en agents.h\n");
        free(valid_pos);
        exit(1);
    }

    printf("EXITO: Se encontraron %d celdas validas.\n", total_valid);

    for (int i = 0; i < count; i++) {
        int idx = rand() % total_valid;
        vehicles[i].row = valid_pos[idx][0];
        vehicles[i].col = valid_pos[idx][1];
        vehicles[i].dir = rand() % 4;
        vehicles[i].stopped = 0;
        vehicles[i].crashed = 0;
    }
    
    free(valid_pos);
}

/* ========================================================== */
/* 2. LÓGICA DEL TRABAJADOR (Hilo)                            */
/* ========================================================== */
void* update_subset(void* arg) {
    ThreadData* d = (ThreadData*)arg;
    unsigned int seed = (unsigned int)(time(NULL) ^ (unsigned long)pthread_self());

    for (int i = d->start; i < d->end; i++) {
        if (d->vehicles[i].crashed) continue;

        int nr = d->vehicles[i].row;
        int nc = d->vehicles[i].col;

        switch (d->vehicles[i].dir) {
            case UP: nr--; break; case DOWN: nr++; break;
            case LEFT: nc--; break; case RIGHT: nc++; break;
        }

        // Límites y Muros
        if (nr < 0 || nr >= d->rows || nc < 0 || nc >= d->cols || d->grid[nr][nc] == '#') {
            d->vehicles[i].dir = opposite(d->vehicles[i].dir);
            continue;
        }

        // Semáforos
        if (d->grid[nr][nc] == '+' && !light_allows(d->lights, d->light_count, nr, nc, d->vehicles[i].dir)) {
            d->vehicles[i].stopped = 1;
            continue;
        }

        // Colisiones (Zona Crítica con Mutex)
        int other = find_vehicle_at(nr, nc, d->vehicles, d->count);
        if (other != -1 && other != i) {
            pthread_mutex_lock(&collision_lock);
            d->vehicles[i].crashed = 1;
            d->vehicles[other].crashed = 1;
            pthread_mutex_unlock(&collision_lock);
            continue;
        }

        // Actualizar posición
        d->vehicles[i].row = nr;
        d->vehicles[i].col = nc;
        d->vehicles[i].stopped = 0;
    }
    return NULL;
}

/* ========================================================== */
/* 3. ACTUALIZACIÓN PRINCIPAL (Paralelismo Explícito)         */
/* ========================================================== */
void update_vehicles(Vehicle *vehicles, int count, char grid[MAX_ROWS][MAX_COLS],
                     TrafficLight *lights, int light_count, int rows, int cols, int num_threads) {
    
    pthread_t threads[num_threads];
    ThreadData tdata[num_threads];
    int step = count / num_threads;

    for (int i = 0; i < num_threads; i++) {
        tdata[i].start = i * step;
        tdata[i].end = (i == num_threads - 1) ? count : (i + 1) * step;
        tdata[i].vehicles = vehicles;
        tdata[i].count = count;
        tdata[i].grid = grid;
        tdata[i].lights = lights;
        tdata[i].light_count = light_count;
        tdata[i].rows = rows;
        tdata[i].cols = cols;

        pthread_create(&threads[i], NULL, update_subset, &tdata[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

/* --- Funciones Auxiliares --- */
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

static Direction opposite(Direction d) {
    switch (d) {
        case UP: return DOWN; case DOWN: return UP;
        case LEFT: return RIGHT; case RIGHT: return LEFT;
        default: return d;
    }
}