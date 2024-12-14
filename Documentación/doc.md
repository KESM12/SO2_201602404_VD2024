# Manual de Usuario

## Introducción
Comprender y manipular las funcionalidades internas del kernel es clave para optimizar el rendimiento y saber como funcionan los SO. El objetivo de este proyecto fue desarrollar módulos del kernel Linux mint que proporcionen información del sistema, tales como estadísticas del cpu, el almacenamiento y la ram.

## Objetivos del Proyecto
- Desarrollar modulos del kernel de linux para recopilar estadísticas del sistema.
- Exponer la información del sistema de forma accesible.
- Documentar y validar el funcionamiento de los módulos.

## Configuración del Entorno
Para terminos del proyecto se utilizo un kernel (linux 6.8-1-usac1) modificado en base a un kernel base (linux 6.8), para validar esto se descargo el kernel desde el sitio oficial, se descomprimio y se siguieron los pasos proporcionados por el auxiliar para la instalación del nuevo kernel.

## Entorno configurado
![Configuración del Entorno](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/kernel.png)  
validación del kernel (configuración adjunta). [Configuracion Kernel](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/configKernel.md)

![Configuración de carpetas](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/entorno1.png)
Estructura de las carpetas para el proyecto.

Entre los archivos que se utilizaron para el proyecto podemos mencionar:

    - syscall_64.tbl (Tabla donde se almacenan el registro de las syscalls)
    - main.c (Archivo donde se coloco el mensaje de bienvenida.)
    - open.c y read_write.c (Archivo donde se coloco el contador para la syscall2)
    - syscalls.h (Archivo donde se declararon las syscalls para sus structs)
    - uts.h (Archivo donde se modifico el nombre de quien modifico el kernel)
    - Kernel, usac, Makefile y sys.c (todos estos componentes fueron modificados para que las syscalls cumplieran su objetivo)

Cada uno de los archivos anteriores tuvieron su modificación la cual se detalla más adelante.

## Descripción de Modificaciones en el Kernel

### Modificaciones en el archivo main.c  
Estas modificaciones fueron mas visuales que otra cosa pero serviran como una bandera. 

Se agrego una bienvenida al sistema.
```bash 
pr_notice("Bienvenido a USAC Linux - Nombre: Kevin Estuardo Secaida Molina, Carnet: 201602404\n");
```
y se agrego un "print" en modo de alerta para que fuera visible al momento de iniciar el sistema.
```bash 
	printk(KERN_ALERT "Kevin Estuardo Secaida Molina - 201602404\n");
	printk(KERN_ALERT "Sistemas Operativos 2\n");
	printk(KERN_ALERT " /$$$$$$$$  /$$$$$$  /$$$$$$$   /$$$$$$ \n");
	printk(KERN_ALERT "|__  $$__/ /$$__  $$| $$__  $$ /$$__  $$\n");
	printk(KERN_ALERT "   | $$   | $$  \\ $$| $$  \\ $$| $$  \\ $$\n");
	printk(KERN_ALERT "   | $$   | $$$$$$$$| $$$$$$$/| $$  | $$\n");
	printk(KERN_ALERT "   | $$   | $$__  $$| $$__  $$| $$  | $$\n");
	printk(KERN_ALERT "   | $$   | $$  | $$| $$  \\ $$| $$  | $$\n");
	printk(KERN_ALERT "   | $$   | $$  | $$| $$  | $$|  $$$$$$/\n");
	printk(KERN_ALERT "   |__/   |__/  |__/|__/  |__/ \\______/ \n");
```

### Modificaciones a la tabla de las syscalls (syscall_64.tbl).
Al final del documento se agregaron las llamadas al sistemas que realizamos (se mencionan mas adelante).
```bash
548 common capture_memory_snapshot sys_capture_memory_snapshot
549 common track_syscall_usage sys_track_syscall_usage
550 common get_io_throttle sys_get_io_throttle
```
Estas seran la referencia que el sistema buscara al momento de ser llamadas en tiempo de ejecución.

### Modificaciones en syscalls.h.
Se agregaron los asmlinkage.
```bash
asmlinkage long sys_capture_memory_snapshot(struct mem_snapshot *snapshot); //memoria
asmlinkage long sys_track_syscall_usage(void);
asmlinkage long sys_get_io_throttle(pid_t pid, struct io_stats *stats); //leer los datos io
```
### Modificaciones en uts.h
Se modifico el nombre del sistema.
```bash
    #define UTS_SYSNAME "Linux Modificado por Kevin Secaida"
```
### Modificaciones a kernel (carpeta).
Estructura:  
![Estructrua del kernel](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/estructurakernel.png)  
En este entorno se crearon las syscall del sistema `memory_snapshot.c`, `track_syscalls.c`, `io_throttle.c`, dentro de la carpeta `include` se agrego el archivo `tracksyscalls.h` el cual seria para realizar importaciones en otros archivos cuando se requiera llamar a track_syscalls.c. 

Código de memory_snapshot.c.
```bash
struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
    unsigned long swap_cached;
    unsigned long swap_used;
};

SYSCALL_DEFINE1(capture_memory_snapshot, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot snap;
    struct sysinfo si;

    si_meminfo(&si);

    snap.total_memory = si.totalram << (PAGE_SHIFT - 10); // Convert to KB
    snap.free_memory = si.freeram << (PAGE_SHIFT - 10);
    snap.active_pages = global_node_page_state(NR_ACTIVE_ANON) + 
                        global_node_page_state(NR_ACTIVE_FILE);
    snap.inactive_pages = global_node_page_state(NR_INACTIVE_ANON) + 
                          global_node_page_state(NR_INACTIVE_FILE);
    if (copy_to_user(snapshot, &snap, sizeof(snap)))
        return -EFAULT;

    return 0;
}
```

Código de tracksyscalls.c.
```bash
struct syscall_usage
{
    unsigned long count;
    struct timespec64 time_last_used;
} __attribute__((aligned(8)));

\#define MAX_SYS_CALLS 4

static struct syscall_usage *syscall_counters;

static int init_syscall_counters(void)
{
    syscall_counters = kzalloc(sizeof(struct syscall_usage) * MAX_SYS_CALLS, GFP_KERNEL);
    return syscall_counters ? 0 : -ENOMEM;
}

void track_syscall(int syscall_id)
{
    struct timespec64 now;

    if (!syscall_counters || syscall_id >= MAX_SYS_CALLS)
    {
        return;
    }

    syscall_counters[syscall_id].count++;
    ktime_get_real_ts64(&now);
    syscall_counters[syscall_id].time_last_used = now;
}

SYSCALL_DEFINE1(track_syscall_usage, struct syscall_usage __user *, statistics)
{
    if (!syscall_counters)
    {
        if (init_syscall_counters() != 0)
        {
            return -ENOMEM;
        }
    }
    int resultadoCopia = copy_to_user(statistics, syscall_counters, sizeof(struct syscall_usage) * MAX_SYS_CALLS);
    if (resultadoCopia)
    {
        return -EFAULT;
    }
    return 0;
}
```

Código de tracksyscalls.h.
```bash
\#ifndef _LINUX_SYSCALL2_H
\#define _LINUX_SYSCALL2_H

void track_syscall(int syscall_id);

#endif
```

Código de io_throttle.c.
```bash
struct io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long read_syscalls;
    unsigned long write_syscalls;
    unsigned long io_wait_time; 
};

SYSCALL_DEFINE2(get_io_throttle, pid_t, pid, struct io_stats __user *, stats)
{
    struct task_struct *task;
    struct io_stats io_stat;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }

    io_stat.bytes_read = task->ioac.read_bytes;
    io_stat.bytes_written = task->ioac.write_bytes;
    io_stat.read_syscalls = task->ioac.syscr;
    io_stat.write_syscalls = task->ioac.syscw;
    io_stat.io_wait_time = 0; 
    rcu_read_unlock();

    if (copy_to_user(stats, &io_stat, sizeof(io_stat)))
        return -EFAULT;

    return 0;
}
```

Tambien se modifico el archivo Makefile agregando la ruta de acceso donde estan los archivos mencionados anteriormente.
```bash
obj-y += usac/
```
### Modulos del sistema.
Dado que uno de los objetivos en el proyecto era desarrollar modulos para monitorear diversas funcionalidades, pues se crearon los modulos para la visualización del CPU, RAM y tamaño de partición o disco duro.

#### Módulo de CPU.
Módulo creado con el objetivo de recuperar el porcentaje (%) utilizado de CPU.
```bash
static void get_cpu_times(unsigned long long *total, unsigned long long *idle)
{
    int cpu;
    *total = 0;
    *idle = 0;

    for_each_possible_cpu(cpu)
    {
        struct kernel_cpustat *kcs = &kcpustat_cpu(cpu);
        u64 user = kcs->cpustat[CPUTIME_USER];
        u64 nice = kcs->cpustat[CPUTIME_NICE];
        u64 system = kcs->cpustat[CPUTIME_SYSTEM];
        u64 idle_time = kcs->cpustat[CPUTIME_IDLE];
        u64 iowait = kcs->cpustat[CPUTIME_IOWAIT];
        u64 irq = kcs->cpustat[CPUTIME_IRQ];
        u64 softirq = kcs->cpustat[CPUTIME_SOFTIRQ];
        u64 steal = kcs->cpustat[CPUTIME_STEAL];
        u64 guest = kcs->cpustat[CPUTIME_GUEST];
        u64 guest_nice = kcs->cpustat[CPUTIME_GUEST_NICE];

        u64 total_cpu = user + nice + system + idle_time + iowait + irq + softirq + steal + guest + guest_nice;

        *total += total_cpu;
        *idle += idle_time;
    }
}

// Función de escritura al archivo /proc
static int escribir_a_proc(struct seq_file *file_proc, void *v)
{
    unsigned long long total, idle;
    unsigned long long delta_total, delta_idle;
    unsigned int usage = 0; // Uso de CPU en centésimas de porcentaje

    // Obtener los tiempos actuales de CPU
    get_cpu_times(&total, &idle);

    if (prev_total != 0 && prev_idle != 0)
    {
        // Calcular las diferencias desde la última lectura
        delta_total = total - prev_total;
        delta_idle = idle - prev_idle;

        if (delta_total > 0)
        {
            // Calcular el porcentaje de uso de la CPU (en centésimas para evitar punto flotante)
            usage = (10000 * (delta_total - delta_idle)) / delta_total; // Uso en centésimas
        }
    }

    // Actualizar los valores anteriores para la próxima lectura
    prev_total = total;
    prev_idle = idle;

    // Escribir el resultado en el archivo /proc
    seq_printf(file_proc, "{\n\"Uso del CPU\": %u.%02u%%\n}\n", usage / 100, usage % 100);

    return 0;
}
```

#### Módulo de la RAM.
Módulo desarrollado para la obtención de diferentes parametros entre ellos : totalRam, memoriaEnUso, porcentaje, libre.
```bash
static int escribir_a_proc(struct seq_file *file_proc, void *v)
{
    unsigned long total, used, free, porcentaje;
    si_meminfo(&inf);

    total = inf.totalram * (unsigned long long)inf.mem_unit;
    used = total - (inf.freeram + inf.bufferram + inf.sharedram) * (unsigned long long)inf.mem_unit;
    free = inf.freeram * (unsigned long long)inf.mem_unit;
    porcentaje = (used * 100) / total;

    seq_printf(file_proc, "{\"totalRam\":%lu, \"memoriaEnUso\":%lu, \"porcentaje\":%lu, \"libre\":%lu }\n",
               total, used, porcentaje, free);
    return 0;
}

static int abrir_aproc(struct inode *inode, struct file *file)
{
    return single_open(file, escribir_a_proc, NULL);
}

static const struct proc_ops archivo_operaciones = {
    .proc_open = abrir_aproc,
    .proc_read = seq_read,
};

static int __init modulo_init(void)
{
    proc_create("ram_so2_201602404", 0, NULL, &archivo_operaciones);
    printk(KERN_INFO "Modulo RAM montado\n");
    return 0;
}
```

### Módulo del storage.
Módulo desarollado para la obtención de almacenamiento en una partición o disco duro.
```bash
static int write_storage_stats(struct seq_file *file, void *v)
{
    struct kstatfs stats;
    struct path path;

    // Resolver la ruta de la partición
    if (kern_path(partition, LOOKUP_FOLLOW, &path))
    {
        seq_printf(file, "{\n\"error\": \"No se pudo acceder a la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Obtener estadísticas de la partición
    if (vfs_statfs(&path, &stats))
    {
        seq_printf(file, "{\n\"error\": \"No se pudieron obtener estadísticas de la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Calcular espacio total y libre en gigabytes
    unsigned long long total_space_gb = (stats.f_blocks * stats.f_bsize) >> 30; // Dividir entre 1024^3
    unsigned long long free_space_gb = (stats.f_bfree * stats.f_bsize) >> 30;  // Dividir entre 1024^3
    unsigned long long available_space_gb = (stats.f_bavail * stats.f_bsize) >> 30; // Dividir entre 1024^3

    // Escribir la salida en formato JSON
    seq_printf(file, "{\n");
    seq_printf(file, "\"partition\": \"%s\",\n", partition);
    seq_printf(file, "\"total_space_gb\": %llu,\n", total_space_gb);
    seq_printf(file, "\"free_space_gb\": %llu,\n", free_space_gb);
    seq_printf(file, "\"available_space_gb\": %llu\n", available_space_gb);
    seq_printf(file, "}\n");

    return 0;
}

static int open_storage_stats(struct inode *inode, struct file *file)
{
    return single_open(file, write_storage_stats, NULL);
}

static const struct proc_ops storage_stats_ops = {
    .proc_open = open_storage_stats,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init storage_stats_init(void)
{
    // Crear el archivo en /proc
    struct proc_dir_entry *entry = proc_create("disk_so2_201602404", 0444, NULL, &storage_stats_ops);
    if (!entry)
    {
        pr_err("Error al crear el archivo /proc/disk_so2_201602404\n");
        return -ENOMEM;
    }

    pr_info("Módulo de estadísticas de almacenamiento cargado. Partición: %s\n", partition);
    return 0;
}
```

## Pruebas Realizadas
Para las pruebas realizas se utilizo el entorno ya de mint cargado propriamente, se realizaron pruebas a las syscalls en el entorno de usuario y al modulo recurriendo a la carpeta /proc y validando su contenido con el comando cat.

Código de la prueba de syscall para ram.
Como se pude observar se llama directamente al identificador (`548`) de la syscall creada por uno este pid es el que esta registrado en la tabla del archivo syscalls_64.tbl 
```bash
struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
};

int main() {
    struct mem_snapshot snapshot;
    if (syscall(548, &snapshot) == 0) {
        printf("Total Memory: %lu KB\n", snapshot.total_memory);
        printf("Free Memory: %lu KB\n", snapshot.free_memory);
        printf("Active Pages: %lu\n", snapshot.active_pages);
        printf("Inactive Pages: %lu\n", snapshot.inactive_pages);
    } else {
        perror("Syscall failed");
    }
    return 0;
}
```

Código de la prueba de syscall de tracker.
```bash
struct syscall_usage {
    unsigned long count;                    
    struct timespec64 time_last_used;       
};

#define MAX_SYS_CALLS 4
#define __NR_track_syscall_usage 549

int main() {
    struct syscall_usage statistics[MAX_SYS_CALLS];

    if (syscall(__NR_track_syscall_usage, statistics) < 0) {
        perror("Error en racso_track_syscall_usage");
        return EXIT_FAILURE;
    }

    // Imprimir resultados
    for (int i = 0; i < MAX_SYS_CALLS; i++) {
        printf("Syscall ID %d: Count = %lu\n", i, statistics[i].count);
        printf("Última vez usada: %ld.%09ld\n", 
               statistics[i].time_last_used.tv_sec, 
               statistics[i].time_last_used.tv_nsec);
    }

    return EXIT_SUCCESS;
}
```

Código de la prueba de syscall de io_throttle.
Para este se utilizo un enfoque diferente, ya que se define el númeor de syscall en lugar de llamarse directo, pero la funcionalidad es la misma esto se hizo con el puro proposito de aprendizaje.
```bash
\#define SYSCALL_GET_IO_THROTTLE 550 

struct io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long read_syscalls;
    unsigned long write_syscalls;
    unsigned long io_wait_time; 
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return -1;
    }

    pid_t pid = atoi(argv[1]); 
    struct io_stats stats;

    if (syscall(SYSCALL_GET_IO_THROTTLE, pid, &stats) == -1) {
        perror("Error en get_io_throttle");
        return errno;
    }

    printf("Estadísticas de I/O para PID %d:\n", pid);
    printf("Bytes leídos: %lu\n", stats.bytes_read);
    printf("Bytes escritos: %lu\n", stats.bytes_written);
    printf("Syscalls de lectura: %lu\n", stats.read_syscalls);
    printf("Syscalls de escritura: %lu\n", stats.write_syscalls);

    return 0;
}
```
![Pruebas de las syscalls](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/pruebasUsuario.png)

Código de la prueba del módulo de cpu.
El siguiente código fue desarrollado para validar el porcentaje de cpu.
```bash
// Variables para almacenar los tiempos anteriores
static unsigned long long prev_total = 0;
static unsigned long long prev_idle = 0;

// Función para obtener los tiempos totales e inactivos de CPU
static void get_cpu_times(unsigned long long *total, unsigned long long *idle)
{
    int cpu;
    *total = 0;
    *idle = 0;

    for_each_possible_cpu(cpu)
    {
        struct kernel_cpustat *kcs = &kcpustat_cpu(cpu);
        u64 user = kcs->cpustat[CPUTIME_USER];
        u64 nice = kcs->cpustat[CPUTIME_NICE];
        u64 system = kcs->cpustat[CPUTIME_SYSTEM];
        u64 idle_time = kcs->cpustat[CPUTIME_IDLE];
        u64 iowait = kcs->cpustat[CPUTIME_IOWAIT];
        u64 irq = kcs->cpustat[CPUTIME_IRQ];
        u64 softirq = kcs->cpustat[CPUTIME_SOFTIRQ];
        u64 steal = kcs->cpustat[CPUTIME_STEAL];
        u64 guest = kcs->cpustat[CPUTIME_GUEST];
        u64 guest_nice = kcs->cpustat[CPUTIME_GUEST_NICE];

        u64 total_cpu = user + nice + system + idle_time + iowait + irq + softirq + steal + guest + guest_nice;

        *total += total_cpu;
        *idle += idle_time;
    }
}

// Función de escritura al archivo /proc
static int escribir_a_proc(struct seq_file *file_proc, void *v)
{
    unsigned long long total, idle;
    unsigned long long delta_total, delta_idle;
    unsigned int usage = 0; // Uso de CPU en centésimas de porcentaje

    // Obtener los tiempos actuales de CPU
    get_cpu_times(&total, &idle);

    if (prev_total != 0 && prev_idle != 0)
    {
        // Calcular las diferencias desde la última lectura
        delta_total = total - prev_total;
        delta_idle = idle - prev_idle;

        if (delta_total > 0)
        {
            // Calcular el porcentaje de uso de la CPU (en centésimas para evitar punto flotante)
            usage = (10000 * (delta_total - delta_idle)) / delta_total; // Uso en centésimas
        }
    }

    // Actualizar los valores anteriores para la próxima lectura
    prev_total = total;
    prev_idle = idle;

    // Escribir el resultado en el archivo /proc
    seq_printf(file_proc, "{\n\"Uso del CPU\": %u.%02u%%\n}\n", usage / 100, usage % 100);

    return 0;
}

// Función de apertura del archivo /proc
static int abrir_aproc(struct inode *inode, struct file *file)
{
    return single_open(file, escribir_a_proc, NULL);
}

// Estructura de operaciones del archivo /proc
static const struct proc_ops archivo_operaciones = {
    .proc_open    = abrir_aproc,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// Función de inicialización del módulo
static int __init modulo_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create("cpu_so2_201602404", 0444, NULL, &archivo_operaciones);
    if (!entry)
    {
        printk(KERN_ERR "cpu_so2_201602404: Fallo al crear /proc/cpu_so2_201602404\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "cpu_so2_201602404: Módulo cargado correctamente\n");
    return 0;
}

// Función de limpieza del módulo
static void __exit modulo_cleanup(void)
{
    remove_proc_entry("cpu_so2_201602404", NULL);
    printk(KERN_INFO "cpu_so2_201602404: Módulo removido correctamente\n");
}

module_init(modulo_init);
module_exit(modulo_cleanup);
```

Código de la prueba del módulo de la ram.
El siguiente código se desarrollo para validar diferentes campos del ram.
```bash
struct sysinfo inf;

static int escribir_a_proc(struct seq_file *file_proc, void *v)
{
    unsigned long total, used, free, porcentaje;
    si_meminfo(&inf);

    total = inf.totalram * (unsigned long long)inf.mem_unit;
    used = total - (inf.freeram + inf.bufferram + inf.sharedram) * (unsigned long long)inf.mem_unit;
    free = inf.freeram * (unsigned long long)inf.mem_unit;
    porcentaje = (used * 100) / total;

    seq_printf(file_proc, "{\"totalRam\":%lu, \"memoriaEnUso\":%lu, \"porcentaje\":%lu, \"libre\":%lu }\n",
               total, used, porcentaje, free);
    return 0;
}

static int abrir_aproc(struct inode *inode, struct file *file)
{
    return single_open(file, escribir_a_proc, NULL);
}

static const struct proc_ops archivo_operaciones = {
    .proc_open = abrir_aproc,
    .proc_read = seq_read,
};

static int __init modulo_init(void)
{
    proc_create("ram_so2_201602404", 0, NULL, &archivo_operaciones);
    printk(KERN_INFO "Modulo RAM montado\n");
    return 0;
}

static void __exit modulo_cleanup(void)
{
    remove_proc_entry("ram_so2_201602404", NULL);
    printk(KERN_INFO "Modulo RAM eliminado \n");
}
```

Código de la prueba del módulo del storage.
El siguiente código tiene como objetivo la validación del espacio en una partición.
```bash
static char *partition = "/"; // Partición por defecto
module_param(partition, charp, 0444);
MODULE_PARM_DESC(partition, "Ruta de la partición (por ejemplo, / o /mnt)");

static int write_storage_stats(struct seq_file *file, void *v)
{
    struct kstatfs stats;
    struct path path;

    // Resolver la ruta de la partición
    if (kern_path(partition, LOOKUP_FOLLOW, &path))
    {
        seq_printf(file, "{\n\"error\": \"No se pudo acceder a la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Obtener estadísticas de la partición
    if (vfs_statfs(&path, &stats))
    {
        seq_printf(file, "{\n\"error\": \"No se pudieron obtener estadísticas de la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Calcular espacio total y libre en gigabytes
    unsigned long long total_space_gb = (stats.f_blocks * stats.f_bsize) >> 30; // Dividir entre 1024^3
    unsigned long long free_space_gb = (stats.f_bfree * stats.f_bsize) >> 30;  // Dividir entre 1024^3
    unsigned long long available_space_gb = (stats.f_bavail * stats.f_bsize) >> 30; // Dividir entre 1024^3

    // Escribir la salida en formato JSON
    seq_printf(file, "{\n");
    seq_printf(file, "\"partition\": \"%s\",\n", partition);
    seq_printf(file, "\"total_space_gb\": %llu,\n", total_space_gb);
    seq_printf(file, "\"free_space_gb\": %llu,\n", free_space_gb);
    seq_printf(file, "\"available_space_gb\": %llu\n", available_space_gb);
    seq_printf(file, "}\n");

    return 0;
}

static int open_storage_stats(struct inode *inode, struct file *file)
{
    return single_open(file, write_storage_stats, NULL);
}

static const struct proc_ops storage_stats_ops = {
    .proc_open = open_storage_stats,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init storage_stats_init(void)
{
    // Crear el archivo en /proc
    struct proc_dir_entry *entry = proc_create("disk_so2_201602404", 0444, NULL, &storage_stats_ops);
    if (!entry)
    {
        pr_err("Error al crear el archivo /proc/disk_so2_201602404\n");
        return -ENOMEM;
    }

    pr_info("Módulo de estadísticas de almacenamiento cargado. Partición: %s\n", partition);
    return 0;
}

static void __exit storage_stats_exit(void)
{
    // Remover el archivo de /proc
    remove_proc_entry("disk_so2_201602404", NULL);
    pr_info("Módulo de estadísticas de almacenamiento descargado\n");
}

```
![Pruebas de los modulos](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/modulos.png)

## Reflexión Personal
No mas sigo en inge de sistemas porque me burle mucho de los industriales. 
