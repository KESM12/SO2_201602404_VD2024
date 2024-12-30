# Documentación.

## Introducción

Sistema de limitación de memoria para procesos en el kernel de Linux. Se logra mediante la creación de cuatro syscalls que permiten gestionar límites de memoria para procesos, con funcionalidades de agregar, consultar, actualizar y eliminar límites.

## Funcionalidad

1. **Agregar límite de memoria**: Asigna un límite de memoria a un proceso identificado por su PID.
2. **Consultar procesos limitados**: Recupera una lista de procesos con sus límites actuales.
3. **Actualizar límite de memoria**: Modifica el límite asignado a un proceso.
4. **Eliminar límite de memoria**: Elimina el límite asignado a un proceso, restaurando sus límites normales.

## Syscalls implementadas

### Syscall 1: `taro_add_memory_limit`

**Descripción**: Agrega un límite de memoria a un proceso.

**Parámetros**:
1. `pid_t process_pid`: Identificador del proceso.
2. `size_t memory_limit`: Límite de memoria en bytes.

**Errores posibles**:
- `EINVAL`: Parámetros inválidos.
- `ESRCH`: Proceso no encontrado.
- `ENOMEM`: No hay memoria suficiente.
- `EPERM`: Permisos insuficientes.
- `101`: Proceso ya tiene un límite asignado.

### Syscall 2: `taro_get_memory_limits`

**Descripción**: Consulta los procesos que tienen límites asignados.

**Parámetros**:
1. `struct memory_limitation *u_processes_buffer`: Buffer de salida en espacio de usuario.
2. `size_t max_entries`: Número máximo de procesos a retornar.
3. `int *processes_returned`: Número de procesos devueltos.

**Errores posibles**:
- `EINVAL`: Parámetros inválidos.
- `ENOMEM`: No hay memoria suficiente.
- `EFAULT`: Error al copiar datos al espacio de usuario.

### Syscall 3: `taro_update_memory_limit`

**Descripción**: Actualiza el límite de memoria de un proceso.

**Parámetros**:
1. `pid_t process_pid`: Identificador del proceso.
2. `size_t memory_limit`: Nuevo límite de memoria en bytes.

**Errores posibles**:
- `EINVAL`: Parámetros inválidos.
- `ESRCH`: Proceso no encontrado.
- `EPERM`: Permisos insuficientes.
- `100`: Proceso ya excede el nuevo límite.

### Syscall 4: `taro_remove_memory_limit`

**Descripción**: Elimina el límite de memoria de un proceso, restaurando sus límites normales.

**Parámetros**:
1. `pid_t process_pid`: Identificador del proceso.

**Errores posibles**:
- `EINVAL`: Parámetros inválidos.
- `ESRCH`: Proceso no encontrado.
- `EPERM`: Permisos insuficientes.
- `102`: Proceso no tiene un límite asignado.

## Cronograma.
**Día 1**:
- Creación de estructuras y funciones auxiliares.
- Implementación de la syscall 1.

**Día 2**:
- Implementación de la syscall 2, 3 y 4.
- Pruebas unitarias.
- Corrección de errores.

**Día 3**:
- Pruebas de integración.
- Documentación.

## Errores.
- **Errores**: No se pudo compilar el kernel.
![Error 1](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/ECompilacion1.jpg)
![Error 2](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/ECompilacion2.jpg)
![Error 3](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/ERam.jpg)
  - **Solución**: Se migro a un compilador anterior (CC=gcc12).

## Pruebas.
- **Prueba General**: Se realizó una prueba general de todas las syscalls.
![Prueba General](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/PGeneral.png)

- **Prueba de límite**: Se realizó una prueba de límite de memoria.
![Prueba de límite](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/PAdd.png)

- **Prueba de actualización**: Se realizó una prueba de actualización de límite de memoria.
![Prueba de actualización](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/PUpdate.png)

- **Prueba de eliminación**: Se realizó una prueba de eliminación de límite de memoria.
![Prueba de eliminación](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/PRemove.png)

- **Prueba de consulta**: Se realizó una prueba de consulta de procesos limitados.
![Prueba de consulta](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3nP3/images/PGet.png)


## Conclusione.
El desarrollo del proyecto no estuvo tan dificil, lo que paso en si fue que el compilador empezo a lanzar errores que por la versión de gcc, por la versión de so, la verdad me gusto trabajar en linux mint (solo habia usado ubuntu) pero tiene muchas cosas que aun me hacen pensar que me falta mucho para poder sacarle todo el jugo a ese sistema operativo, pero si al final 3 vm's arruinadas y 2 dias desperdiciados en eso. 

Ahora en si sobre el proyecto me parece que el funcionamiento de las syscalls esta curioso ya que se pueden utilizar para automatizar procesos, a gran escala creo que pase leyendo mas codigos de errores que codificando pero pues, de los erroes se aprende.