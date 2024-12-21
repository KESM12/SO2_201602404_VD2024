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




## 5. Pruebas y Validación
### 5.1 Scripts de Prueba
- **Pruebas para Tamalloc**: Asignador de memoria diseñado para no reservar páginas fisicas inmediatamente, obligandolo a inicializar cada pagina en 0 hasta su primer acceso.
- **Script 1.**: Script en C que hace uso de la syscall 551 para asignar memoria.  
![Script1](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/tamallocScript1.png)

- **Código.**
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
  #define SYS_TAMALLOC 551
  printf("PID: %d asignado a taro_Tamalloc.\n", getpid());
  size_t total_size = 100 * 1024 * 1024; // 100 MB

  // Use the tamalloc syscall
  char *buffer = (char *)syscall(SYS_TAMALLOC, total_size);
  if ((long)buffer < 0) {
    perror("Error en Tamalloc");
    return 1;
  }
  printf("Alojamiento de memoria (%zu bytes) usando Tamalloc en la dirección: %p\n", total_size, buffer);

  for (size_t i = 0; i < total_size; i++) {
    buffer[i] = 0; // Access each page to initialize it
    if (i % (1024 * 1024) == 0 && i > 0) { // Every 1 MB
      printf("%zu MB Comprobados\n", i / (1024 * 1024));
      sleep(1);
    }
  }

  printf("Memoria inicializada correctamente.\n");
  return 0;
}
```


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
Día 1. 19/12/2024
Busqueda de información, implementación de la syscall taro_tamalloc, script 1 realizado. 

Día 2. 20/12/2024
Implementación de código para la syscall de conteo de memoria reservada y memoria utilizada, arreglo de errores (installación de python corrompio el sistema).

[https://community.wandisco.com/s/article/Guide-to-Out-of-Memory-OOM-events-and-decoding-their-logging] 


## 9. Reflexión Personal
- Autoevaluación y aprendizajes adquiridos durante el proyecto.
- Desafíos enfrentados y cómo se resolvieron.
