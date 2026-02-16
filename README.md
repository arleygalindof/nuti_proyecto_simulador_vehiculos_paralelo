---

### 2. README.md para la versión PARALELA

```markdown
# Simulador de Tráfico Optimizado con HPC (OpenMP & Pthreads)

## Descripción
Esta es la implementación de alto rendimiento del simulador de agentes. Se centra en la optimización del motor de actualización de vehículos utilizando dos paradigmas de programación paralela para comparar su eficiencia y escalabilidad.

## Análisis de Desempeño (HPC)
El proyecto incluye un análisis comparativo de métricas de desempeño computacional, evaluando el comportamiento del sistema con 1, 2, 4 y 8 hilos.

### Métricas Obtenidas (3000 Agentes)
| Paradigma | Hilos | Tiempo (s) | Speedup | Eficiencia |
| :--- | :---: | :---: | :---: | :---: |
| Secuencial Puro | 1 | 0.084432 | 1.00x | 100% |
| OpenMP | 1 | 0.003706 | 22.78x | 2278% |
| OpenMP | 8 | 0.002133 | 39.58x | 494% |

## Optimizaciones Implementadas
- **OpenMP:** Paralelización basada en directivas con planificación dinámica (`schedule(dynamic)`) para un balanceo de carga óptimo.
- **Pthreads:** Implementación de hilos POSIX con gestión de exclusión mutua mediante Mutex para zonas críticas de colisión.
- **Thread-Safety:** Sustitución de funciones bloqueantes (como `rand()`) por versiones reentrantes (`rand_r`) para maximizar el throughput.

## Compilación y Ejecución
Para compilar con soporte de OpenMP:
```bash
gcc main.c grid.c traffic_lights.c agents.c visual.c -o traffic_sim -fopenmp
```
Para ejecutar
```bash
./traffic_sim 3000 grid_large.txt 100 4
```
Donde el primer parámetro corresponde al # de Vehículos, el siguiente es el mapa con las rutas , el tercero el # de iteracciones y el último el número de hilos/procesos simultaneos
