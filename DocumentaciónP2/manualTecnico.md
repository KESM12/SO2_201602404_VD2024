# Manual Técnico

## 1. Introducción
Describir brevemente el propósito del manual y el proyecto.
- **Objetivo del proyecto**: Implementación de dos algoritmos de planificación y sincronización en el kernel de Linux.


---

## 2. Requisitos Previos
### 2.1 Software y Herramientas
- **Sistema Operativo**: Linux Mint
- **Versión del Kernel**: Linux 6.8.0-49-usac1.
- **Lenguaje de Programación**: C.
- **Compilador**: GCC.
- **Herramientas de Desarrollo**:
  - `make`
  - VirtualBox
  - Editor de texto (VS Code).

---

## 3. Diseño de la Solución
Para la modificación del kernel y agregar las nuevas funcionalidades se modificaran directamente en el archivo `sched.c` el cual se encuentra linkeado para una mayor facilidad a la hora de modificarlo.

### 3.1 Algoritmos de Planificación
#### FIFO (First-In, First-Out)
- **Descripción**: El algoritmo fifo por sus singlas en ingles se refiere a la forma en que se ejecutaran las tareas, o sea el primero en entrar es el primero en salir. 

La clase `Queue` implementa un envase primero en entrar, primero en salir. Los elementos se agregan a un «final» de la secuencia usando`put()`, y eliminado del otro extremo usando `get()`.

- **Pseudocódigo**:
```python
import queue

q = queue.Queue()

for i in range(5):
    q.put(i)

while not q.empty():
    print(q.get(), end=' ')
print()
```


#### Round Robin con Quantum Dinámico
- **Descripción**:  algoritmo de programación de CPU donde a cada proceso se le asigna cíclicamente un intervalo de tiempo fijo. Es la versión preventiva del algoritmo First come 
First Serve CPU Scheduling.

    - El algoritmo de CPU Round Robin generalmente se centra en la técnica de Time Sharing.

    - El período de tiempo durante el cual se permite que un proceso o trabajo se ejecute en un método preventivo se llama tiempo Quantum.

    -  Cada proceso o trabajo presente en la cola lista se le asigna la CPU para ese Quantum, si la ejecución del proceso se completa durante ese tiempo, entonces el proceso lo hará final de lo contrario, el proceso volverá a la mesa de espera y espere su próximo turno para completar la ejecución. 
- **Pseudocódigo**.
```python
def findWaitingTime(processes, n, bt, wt, quantum):
    rem_bt = [0]*n
    for i in range(n):
        rem_bt[i] = bt[i]
    t = 0
    while(1):
        done = True
        for i in range(n):
            if (rem_bt[i]>0):
                done = False
                if (rem_bit[i]>quantum):
                    t += quantum
                    rem_bt[i]-= quantum
                else: 
                    t = t + rem_bt[i]
                    wt[i] = t - bt[i]
                    rem_bt[i] = 0
            if (done == True):
                break

def findTurnAroundTime(processes, n, bt, wt, tat):
    for i in range(n):
        tat[i] = bt[i] + wt[i]

def findavgTime(processes, n, bt, quantum):
    wt = [0] * n
    tat = [0] * n
    findWaitingTime(processes, n, bt, wt, quantum)
    findTurnAroundTime(processes, n, bt, wt, tat)

    print("Processes Burst Time Waiting", "Time Trun-Around Time")
    total_wt = 0
    total_tat = 0
    for i in range(n):
        total_wt = total_wt + wt[i]
        total_tat = total_tat + tat[i]
        print(" ", i + 1, "\t\t", bt[i],
              "\t\t", wt[i], "\t\t", tat[i])
    print("\nAverage waiting time = %.5f " % (total_wt / n))
    print("Average turn around time = %.5f " % (total_tat / n))

if __name__ == "__main__":
    proc = [1, 2, 3]
    n = 3
    burst_time = [10, 5, 8]
    quantum = 2
    findavgTime(proc, n, burst_time, quantum)
```
```
PN          BT   WT      TAT
 1        10     13         23
 2        5     10         15
 3        8     13         21
Average waiting time = 12
Average turn around time = 19.6667
```
### 3.2 Técnicas de Sincronización
- **Objetivo**: Evitar condiciones de carrera y deadlocks.
- **Herramientas usadas**:
  - Mutex.
  - Semáforos. (wait(), signal())
  - Pthreads o kthreads.
- **Implementación**: 
#### Mutex
```c
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
  
pthread_t tid[2]; 
int counter; 
pthread_mutex_t lock; 
  
void* trythis(void* arg) 
{ 
    pthread_mutex_lock(&lock); 
  
    unsigned long i = 0; 
    counter += 1; 
    printf("\n Job %d has started\n", counter); 
  
    for (i = 0; i < (0xFFFFFFFF); i++) 
        ; 
  
    printf("\n Job %d has finished\n", counter); 
  
    pthread_mutex_unlock(&lock); 
  
    return NULL; 
} 
  
int main(void) 
{ 
    int i = 0; 
    int error; 
  
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
  
    while (i < 2) { 
        error = pthread_create(&(tid[i]), 
                               NULL, 
                               &trythis, NULL); 
        if (error != 0) 
            printf("\nThread can't be created :[%s]", 
                   strerror(error)); 
        i++; 
    } 
  
    pthread_join(tid[0], NULL); 
    pthread_join(tid[1], NULL); 
    pthread_mutex_destroy(&lock); 
  
    return 0; 
} 
```
---

## 4. Implementación
### 4.1 Modificaciones del Kernel
- Explicación detallada de los archivos modificados en el kernel.
- **Ubicación del código**:
  - `kernel/sched.c` (ejemplo).
- **Funciones añadidas/modificadas**.
- Fragmentos de código relevantes.

---

## 5. Pruebas y Validación
### 5.1 Scripts de Prueba
- **Pruebas para FIFO**: Descripción y ejemplos.
- **Pruebas para Round Robin**: Descripción y ejemplos.
- **Pruebas de sincronización**: Casos donde se verifica la ausencia de condiciones de carrera y deadlocks.

### 5.2 Resultados Obtenidos
- Análisis del rendimiento de cada algoritmo.
- Comparación de tiempos de respuesta y eficiencia.

---

## 6. Análisis de Resultados
- **Rendimiento**: Impacto en la CPU al usar cada algoritmo.
- **Eficiencia de sincronización**: Validación de que se evitaron problemas de concurrencia.
- **Problemas encontrados y soluciones aplicadas**.

---

## 7. Errores
- **Código fuente completo**: Fragmentos importantes del código.
- **Comandos utilizados**: Scripts Bash/C utilizados durante pruebas.
- **Imágenes/Diagramas**: Capturas de pantalla de resultados o arquitectura.

---

## 8. Cronograma
Día 1 
Busqueda de información  
queue — Implementación FIFO segura para hilos  
https://rico-schmidt.name/pymotw-3/queue/index.html

Round Robin Scheduling for the same Arrival time
https://www.geeksforgeeks.org/program-for-round-robin-scheduling-for-the-same-arrival-time/

Sicronización Mutex  
https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/

Sincronización Semaphore
https://www.geeksforgeeks.org/readers-writers-problem-set-1-introduction-and-readers-preference-solution/

## 9. Reflexión Personal
- Autoevaluación y aprendizajes adquiridos durante el proyecto.
- Desafíos enfrentados y cómo se resolvieron.
