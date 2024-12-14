#include <linux/module.h>        // THIS_MODULE, MODULE_LICENSE, ...
#include <linux/init.h>          // module_init, module_exit
#include <linux/proc_fs.h>       // proc_create, remove_proc_entry
#include <linux/seq_file.h>      // seq_file, single_open, single_release
#include <linux/fs.h>            // struct file
#include <linux/kernel_stat.h>   // kcpustat_cpu, CPUTIME_*
#include <linux/cpu.h>           // for_each_possible_cpu
#include <linux/uaccess.h>       // copy_to_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KESM");
MODULE_DESCRIPTION("Modulo para registrar el porcentaje de uso de CPU");
MODULE_VERSION("1.0");

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
    seq_printf(file_proc, "{\n\"cpu_usage\": %u.%02u%%\n}\n", usage / 100, usage % 100);

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


