#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "grid.h"
#include "traffic_lights.h"
#include "agents.h"

#define MAX_LIGHTS 100
#define CYCLE_TIME 3

#include <omp.h>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Uso: %s <num_vehicles> <grid_file>\n", argv[0]);
        return 1;
    }

    int num_vehicles = atoi(argv[1]);
    char *grid_file = argv[2];
    int steps = atoi(argv[3]);
	
	int num_threads = 0; // 0 = no tocar OpenMP
    if (argc > 4) {
        num_threads = atoi(argv[4]);

        if (num_threads > 0) {
            omp_set_num_threads(num_threads);
        }
    }	

    char grid[MAX_ROWS][MAX_COLS];
    int rows, cols;

    if (!load_grid(grid_file, grid, &rows, &cols)) {
        return 1;
    }

    Vehicle *vehicles = malloc(sizeof(Vehicle) * num_vehicles);

    TrafficLight lights[MAX_LIGHTS];
    int light_count;
    init_traffic_lights(lights, &light_count, grid, rows, cols);

    init_vehicles(vehicles, num_vehicles, grid, rows, cols);

    printf("Simulación iniciada\n");
    printf("Grid: %s\n", grid_file);
    printf("Vehículos: %d\n", num_vehicles);
    printf("Semáforos: %d\n\n", light_count);

	double start = omp_get_wtime(); // Tiempo inicial real
	//#pragma omp parallel for
    for (int step = 0; step < steps; step++) {
        update_traffic_lights(lights, light_count, CYCLE_TIME);
        update_vehicles(vehicles, num_vehicles,
                        grid, lights, light_count,
                        rows, cols);
        //render_simulation(grid, rows, cols, vehicles, num_vehicles);//visualizador, no tenido en cuenta para mediciones
    }

	double end = omp_get_wtime(); // Tiempo final real
	printf("Tiempo Real de Cómputo: %f segundos\n", end - start);

    free(vehicles);
    return 0;
}


