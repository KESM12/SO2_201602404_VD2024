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
![Script1](DocumentacionP2\images\tamallocScript1.png)

- **Código.**


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




## 9. Reflexión Personal
- Autoevaluación y aprendizajes adquiridos durante el proyecto.
- Desafíos enfrentados y cómo se resolvieron.
