# 🚀 Simulación de Tráfico Urbano (Versión Paralela con OpenMP)

## 📌 Descripción

Esta versión extiende el modelo secuencial incorporando paralelismo mediante OpenMP para mejorar el rendimiento computacional.

---

## 🧠 Estrategia de Paralelización

Se implementa:

### ✅ Paralelización por agentes

* Cada hilo procesa un subconjunto de vehículos
* Se paraleliza únicamente la fase de cálculo independiente

```c
#pragma omp parallel for
```

---

## ⚙️ Diseño de ejecución

Cada iteración mantiene:

1. Actualización de semáforos (serial)
2. Cálculo de movimientos (PARALELO)
3. Resolución de conflictos (serial)
4. Actualización de estado (serial)

---

## ⚠️ Decisiones importantes

### ❌ No se paralelizó el tiempo (steps)

Razón:

* Cada iteración depende de la anterior
* Paralelizar steps rompe la lógica del modelo

---

### ❌ No se usó `local_occupancy`

Razón:

* Alto consumo de memoria
* Coste de inicialización
* Uso de `critical` generaba cuello de botella

---

### ✅ Se evitó sincronización pesada

* No se usan locks
* No se usan secciones críticas
* Se maximiza independencia entre hilos

---

## 🧩 Arquitectura

Se mantiene la misma estructura modular:

* `main.c`
* `grid.c`
* `traffic_lights.c`
* `agents.c` (paralelizado)
* `visual.c`

---

## ▶️ Compilación

```bash
gcc main.c grid.c traffic_lights.c agents.c visual.c -o traffic_sim -fopenmp
```

---

## ▶️ Ejecución

```bash
./traffic_sim <vehiculos> <grid> [steps] [threads]
```

Ejemplo:

```bash
./traffic_sim 1000 mapa.txt 1000 4
```

---

## 🎛️ Control de hilos

También se puede usar:

```bash
export OMP_NUM_THREADS=4
```

---

## 📊 Métricas de desempeño

Se mide:

* Tiempo de ejecución (`omp_get_wtime`)
* Speedup
* Eficiencia

---

## 📈 Resultados esperados

* Mejora de rendimiento con mayor número de vehículos
* Escalabilidad limitada por secciones seriales

---

## 🧠 Aprendizajes HPC

* No todo se debe paralelizar
* La sincronización reduce rendimiento
* El overhead puede superar beneficios
* Paralelizar solo partes independientes es clave

---

## ⚠️ Recomendaciones

Para obtener mejores resultados:

* Usar muchos vehículos (≥ 1000)
* Usar múltiples iteraciones
* Ejecutar en CPU multinúcleo

---

## 🎯 Objetivo

Demostrar:

* Mejora de rendimiento frente a la versión secuencial
* Aplicación correcta de paralelismo con OpenMP
* Análisis de eficiencia computacional

---
