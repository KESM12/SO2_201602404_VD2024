# Manual Técnico

## 1. Introducción
Describe el desarrollo e implementación de tres syscalls personalizadas en el kernel de Linux para gestionar y analizar el uso de memoria en el sistema. El proyecto tiene como objetivo principal explorar y modificar las capacidades nativas del kernel, mejorando la gestión de recursos y facilitando el monitoreo de estadísticas de memoria.

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
  - PINCE

---

## 3. Diseño de la Solución
Se realizaron 3 syscalls, con el objetivo de ver el funcionamiento de la memoria en linux mint.

### Syscall 1 (Tamalloc).
Este código define una syscall taro_tamalloc que asigna memoria virtual en el kernel.

Usa do_mmap para mapear memoria virtual sin reservar páginas físicas inicialmente (lazy allocation).
Verificación y retorno:

Verifica si el mapeo fue exitoso.
Imprime un mensaje de éxito y retorna la dirección de memoria virtual asignada.

```c
SYSCALL_DEFINE1(taro_tamalloc, size_t, size) {
  unsigned long addr;
  size_t aligned_size;
  unsigned long populate = 0;  // Para manejar el mapeo inicial
  vm_flags_t vm_flags = 0;     // Bandera de las propiedades de mapeo

  // Verificar si el tamaño solicitado es válido
  if (size == 0) {
    printk(KERN_ERR "taro_tamalloc: Tamaño de memoria no puede ser 0.\n");
    return -EINVAL; // Error de argumento inválido
  }

  // Alinear el tamaño solicitado al tamaño de página
  aligned_size = PAGE_ALIGN(size);

  // Comprobar si la alineación falló
  if (aligned_size == 0) {
    printk(KERN_ERR "taro_tamalloc: Error en la alineación del tamaño.\n");
    return -ENOMEM; // Error de memoria insuficiente
  }

  // Configurar las banderas de protección y tipo de mapeo
  vm_flags = VM_READ | VM_WRITE | VM_MAYREAD | VM_MAYWRITE;

  // Mapear memoria sin reservar páginas físicas (lazy allocation)
  addr = do_mmap(NULL, 0, aligned_size, PROT_READ | PROT_WRITE,
           MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
           vm_flags, 0, &populate, NULL);

  // Verificar si el mapeo fue exitoso
  if (IS_ERR_VALUE(addr)) {
    printk(KERN_ERR "taro_tamalloc: Error al mapear la memoria virtual.\n");
    return addr; // Retornar el código de error
  }

  // Mensaje de éxito
  printk(KERN_INFO "taro_tamalloc: Memoria asignada exitosamente en 0x%lx, tamaño: %zu bytes\n", addr, aligned_size);

  return addr; // Dirección de memoria virtual asignada
}
```

### Syscall 2 (Memoria reserved and committed para un proceso en especifico y para todos).
Este código define una estructura process_mem_stats y una syscall taro_ind_mem_stats para obtener estadísticas de memoria de procesos en el sistema.

Verifica si los argumentos son válidos.
Obtiene el total de páginas de memoria del sistema.
Si pid es mayor a 0, busca el proceso objetivo por su PID y calcula las estadísticas de memoria para ese proceso.
Si pid es 0, recorre todos los procesos del sistema y calcula las estadísticas de memoria para cada uno.
Copia las estadísticas calculadas al buffer de usuario.
Retorna la cantidad de datos copiados o un código de error si ocurre algún problema.
Este código es útil para monitorear el uso de memoria de los procesos en el sistema y puede ser utilizado para análisis de rendimiento o detección de problemas de memoria.
```c
struct process_mem_stats {
    pid_t pid;                    // PID del proceso
    unsigned long reserved_kb;    // Memoria reservada en KB
    unsigned long committed_kb;   // Memoria utilizada en KB
    unsigned long committed_pct;  // Porcentaje utilizado
    int oom_score;                // OOM Score
};

SYSCALL_DEFINE3(taro_ind_mem_stats, pid_t, pid, struct process_mem_stats __user *, stats, size_t, stats_len) {
    struct task_struct *task;
    struct process_mem_stats local_stats;
    struct mm_struct *mm;
    unsigned long total_pages;
    unsigned long badness;
    int oom_score_adj;
    size_t copied = 0;

    if (!stats || stats_len == 0)
        return -EINVAL; // Argumentos inválidos

    // Obtener total de páginas del sistema
    total_pages = totalram_pages();

    if (pid > 0) {
        // Buscar el proceso objetivo por su PID
        rcu_read_lock();
        task = find_task_by_vpid(pid);
        if (!task) {
            rcu_read_unlock();
            return -ESRCH; // Proceso no encontrado
        }
        get_task_struct(task);
        rcu_read_unlock();

        mm = task->mm;
        if (!mm) {
            put_task_struct(task);
            return -EFAULT; // El proceso no tiene memoria asignada
        }

        // Calcular estadísticas
        local_stats.pid = pid;
        local_stats.reserved_kb = (mm->total_vm * PAGE_SIZE) / 1024;
        local_stats.committed_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;
        local_stats.committed_pct = local_stats.reserved_kb > 0
                                        ? (local_stats.committed_kb * 100) / local_stats.reserved_kb
                                        : 0;

        // Calcular OOM Score No salio xd ni modo toco dejarlo así 
        badness = oom_badness(task, total_pages);
        oom_score_adj = task->signal->oom_score_adj;
        badness += (badness * oom_score_adj) / 1000;
        local_stats.oom_score = badness > 1000 ? 1000 : (badness < 0 ? 0 : badness);

        put_task_struct(task);

        // Copiar datos al espacio de usuario
        if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
            return -EFAULT;

        return sizeof(local_stats);
    } else {
        // Recorrer todos los procesos en el sistema
        rcu_read_lock();
        for_each_process(task) {
            if (copied + sizeof(local_stats) > stats_len) {
                rcu_read_unlock();
                return -ENOSPC; // Valida si hay espacio insuficiente en el buffer
            }

            mm = task->mm;
            if (!mm)
                continue;

            // Calcular estadísticas
            local_stats.pid = task->pid;
            local_stats.reserved_kb = (mm->total_vm * PAGE_SIZE) / 1024;
            local_stats.committed_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;
            local_stats.committed_pct = local_stats.reserved_kb > 0
                                            ? (local_stats.committed_kb * 100) / local_stats.reserved_kb
                                            : 0;

            // Calcular OOM Score
            badness = oom_badness(task, total_pages);
            oom_score_adj = task->signal->oom_score_adj;
            badness += (badness * oom_score_adj) / 1000;
            local_stats.oom_score = badness > 1000 ? 1000 : (badness < 0 ? 0 : badness);

            // Copiar datos al espacio de usuario
            if (copy_to_user((void __user *)((char __user *)stats + copied), &local_stats, sizeof(local_stats))) {
                rcu_read_unlock();
                return -EFAULT;
            }

            copied += sizeof(local_stats);
        }
        rcu_read_unlock();

        return copied; // Retorna la cantidad de datos copiados
    }
}
```


### Syscall 3 (Memoria reserved and commited total.)
Valida el puntero de usuario.
Recorre todos los procesos del sistema.
Para cada proceso, obtiene la estructura mm_struct que contiene información de memoria.
Suma la memoria reservada (total_vm) y la memoria comprometida (RSS) en MB.
Copia los totales calculados al buffer de usuario.
Retorna 0 si la syscall es exitosa o un código de error si ocurre algún problema.
Este código es útil para obtener una visión general del uso de memoria en todo el sistema.
```c
// Estructura para devolver los totales de memoria
struct total_mem_stats {
    unsigned long total_reserved_mb;   // Total de memoria reservada en MB
    unsigned long total_committed_mb;  // Total de memoria comprometida en MB
};

// Implementación de la syscall
SYSCALL_DEFINE1(taro_tod_mem_stats, struct total_mem_stats __user *, totals) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct total_mem_stats local_totals = {0}; // Inicializar los totales

    // Validar el puntero de usuario
    if (!totals)
        return -EINVAL; // Argumento inválido

    if (!access_ok(totals, sizeof(struct total_mem_stats)))
        return -EFAULT; // Buffer de usuario inválido

    // Recorrer todos los procesos en el sistema
    rcu_read_lock();
    for_each_process(task) {
        mm = get_task_mm(task);
        if (!mm)
            continue;

        // Sumar memoria reservada y comprometida
        local_totals.total_reserved_mb += (mm->total_vm * PAGE_SIZE) / (1024 * 1024); // Total_VM en MB
        local_totals.total_committed_mb += (get_mm_rss(mm) * PAGE_SIZE) / (1024 * 1024); // RSS en MB

        mmput(mm); // Liberar la referencia de mm_struct
    }
    rcu_read_unlock();

    // Copiar los totales al espacio de usuario
    if (copy_to_user(totals, &local_totals, sizeof(struct total_mem_stats)))
        return -EFAULT;

    return 0; // Syscall exitosa
}
```

## 4. Pruebas y Validación
### 4.1 Scripts de Prueba
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


### 4.2 Resultados Obtenidos
![ResultadoGeneral](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/pruebaGeneral.png)

![ResutadoIndividualSys2](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/pruebaIndividual.png)

![PINCE](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/PINCE0.png)

![PINCE](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/PINCE0.1.png)

---

## 5. Errores
Se encontraron errores al momento de compilar el kernel y al momento de ejecutar el program PINCE.

### Error 1.
![Error1](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/ErrorCompilacion.png)  
![Error2](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/ErrorCompilacion2.png)  
Estos errores se debieron a que habian estructuras que no pertencian al archivo que se estaba ejecutando, solución reescribir el código y compilar de nuevo.

### Error 2.
![Error3](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/ErrorKernel.png)  
Este error se debio a que al momento de instalar python se corrompio el sistema, solución revertir la instalación de python, borrar el cache, los certificados y volver a instalar el kernel.

### Error 3.
![Error4](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/DocumentacionP2/images/ErrorPINCE.png)  
Este error fue un tanto curioso ya que se debio a la modificación del SO, tenia la versión de linux con mi nombre por lo que no se podia ejecutar PINCE, solución cambiar el nombre del SO, se devolvio a la versión original y se pudo ejecutar PINCE.

---

## 6. Cronograma
Día 1. 19/12/2024
Busqueda de información, implementación de la syscall taro_tamalloc, script 1 realizado. 

Día 2. 20/12/2024
Implementación de código para la syscall de conteo de memoria reservada y memoria utilizada, arreglo de errores (installación de python corrompio el sistema).

[https://community.wandisco.com/s/article/Guide-to-Out-of-Memory-OOM-events-and-decoding-their-logging]

Día 3. 21/12/2024
Se busco una solución para el score de oom.
[https://stackoverflow.com/questions/38597965/difference-between-resident-set-size-rss-and-java-total-committed-memory-nmt]
[https://lwn.net/Articles/391222/]
[https://lwn.net/Articles/548180/]

Día 4. 22/12/2024
Implementación de las syscalls 2 y 3 con el defecto de que no se logro concretar el score de oom.

Día 5. 23/12/2024
Realización de pruebas unitarias y conjuntas a las syscalls nuevas.

Se encontro un error al momento de intentar ejecutar PINCE, se leyo el siguiente foro para solucionarlo.
[https://forums.linuxmint.com/viewtopic.php?t=400193]


## 7. Reflexión Personal
- Aunque quisiera decir que se logro el objetivo no estoy contento con mi desarrollo durante este proyecto ya que hubieron varias cosas que me hubieran gustado entender mas a profundidad, como el oom_score no tanto su forma de funcionar si no su forma de obtenerse dentro del kernel, realmente una de las experiencias mas satisfactorias fue el poder recuperar en mas de una ocación el kernel, entendiendo los puntos de cuando se llena la ram o alguno de los procesos llega a ocupar mas de lo requerido, evitando los crasheos al sistema y visualizando logs para poder corregir errores de ejecución de programas.
- Entre los desafios mas nuevos esta el de no tener los recursos aun sabiendo que si los tengo el detalle es que hay cosas que uno no ve y que por ende es mejor validar si no hay alguien que ya haya tenido ese mismo problema, al visitar varios foros me di cuenta que estaban en ingles pero que es muy raro que alguien no comparta su problema y que no haya una solución, por lo que leer foros es una buena opción auqnue siempre hay que tomar en cuenta la documentación oficial.
