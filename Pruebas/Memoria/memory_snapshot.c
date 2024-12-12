// #include <linux/init.h>
// #include <linux/module.h>
// #include <linux/proc_fs.h>
// #include <linux/seq_file.h>
// #include <linux/mm.h>
// #include <linux/vmstat.h>
// #include <linux/sched.h>
// #include <linux/kernel.h>
// #include <linux/uaccess.h>

// struct mem_snapshot {
//     unsigned long total_memory;   // in KB
//     unsigned long free_memory;    // in KB
//     unsigned long active_pages;
//     unsigned long inactive_pages;
// };

// // Estructura para el snapshot de memoria
// struct mem_snapshot {
//     unsigned long total_memory;
//     unsigned long free_memory;
//     unsigned long active_pages;
//     unsigned long inactive_pages;
// };

// // Variable global para almacenar el último snapshot
// static struct mem_snapshot last_snapshot;

// // Declaración de la syscall
// extern long sys_capture_memory_snapshot(struct mem_snapshot *snapshot);

// // Actualiza los datos del último snapshot usando la syscall personalizada
// static void update_memory_snapshot(void)
// {
//     long ret = sys_capture_memory_snapshot(&last_snapshot);
//     if (ret != 0) {
//         pr_err("memory_snapshot_kesm: Syscall failed with error %ld\n", ret);
//     }
// }

// // Función para manejar la lectura del archivo en /proc
// static int memory_snapshot_show(struct seq_file *m, void *v)
// {
//     update_memory_snapshot();
//     seq_printf(m, "Total Memory: %lu KB\n", last_snapshot.total_memory);
//     seq_printf(m, "Free Memory: %lu KB\n", last_snapshot.free_memory);
//     seq_printf(m, "Active Pages: %lu\n", last_snapshot.active_pages);
//     seq_printf(m, "Inactive Pages: %lu\n", last_snapshot.inactive_pages);
//     return 0;
// }

// static int memory_snapshot_open(struct inode *inode, struct file *file)
// {
//     return single_open(file, memory_snapshot_show, NULL);
// }

// static const struct proc_ops memory_snapshot_fops = {
//     .proc_open    = memory_snapshot_open,
//     .proc_read    = seq_read,
//     .proc_lseek   = seq_lseek,
//     .proc_release = single_release,
// };

// // Funciones de inicialización y limpieza
// static int __init memory_snapshot_init(void)
// {
//     if (!proc_create("memory_snapshot_kesm", 0444, NULL, &memory_snapshot_fops)) {
//         pr_err("memory_snapshot_kesm: Failed to create /proc/memory_snapshot_kesm\n");
//         return -ENOMEM;
//     }
//     pr_info("memory_snapshot_kesm: Created /proc/memory_snapshot_kesm\n");
//     return 0;
// }

// static void __exit memory_snapshot_exit(void)
// {
//     remove_proc_entry("memory_snapshot_kesm", NULL);
//     pr_info("memory_snapshot_kesm: Removed /proc/memory_snapshot_kesm\n");
// }

// module_init(memory_snapshot_init);
// module_exit(memory_snapshot_exit);

// MODULE_LICENSE("GPL");
// MODULE_AUTHOR("Kevin Secaida");
// MODULE_DESCRIPTION("Proc interface for memory snapshot");




#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/mm.h>

// Definición de la estructura para la información de memoria
struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
};

// Variable global para almacenar el último snapshot
static struct mem_snapshot last_snapshot;

// Nombre del archivo en /proc
#define PROC_FILE_NAME "memory_snapshot_kesm"

// Función para mostrar los datos en /proc
static int memory_snapshot_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Total Memory: %lu KB\n", last_snapshot.total_memory);
    seq_printf(m, "Free Memory: %lu KB\n", last_snapshot.free_memory);
    seq_printf(m, "Active Pages: %lu\n", last_snapshot.active_pages);
    seq_printf(m, "Inactive Pages: %lu\n", last_snapshot.inactive_pages);
    return 0;
}

// Llamada a la syscall para actualizar `last_snapshot`
static void update_memory_snapshot(void)
{
    long ret = syscall(548, &last_snapshot);
    if (ret != 0) {
        pr_err("memory_snapshot_kesm: Syscall failed with error %ld\n", ret);
    }
}

// Función open para inicializar la lectura en /proc
static int memory_snapshot_open(struct inode *inode, struct file *file)
{
    update_memory_snapshot();
    return single_open(file, memory_snapshot_show, NULL);
}

// Operaciones del archivo en /proc
static const struct proc_ops memory_snapshot_fops = {
    .proc_open    = memory_snapshot_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// Función de inicialización del módulo
static int __init memory_snapshot_init(void)
{
    if (!proc_create(PROC_FILE_NAME, 0444, NULL, &memory_snapshot_fops)) {
        pr_err("memory_snapshot_kesm: Failed to create /proc/%s\n", PROC_FILE_NAME);
        return -ENOMEM;
    }
    pr_info("memory_snapshot_kesm: Created /proc/%s\n", PROC_FILE_NAME);
    return 0;
}

// Función de limpieza del módulo
static void __exit memory_snapshot_exit(void)
{
    remove_proc_entry(PROC_FILE_NAME, NULL);
    pr_info("memory_snapshot_kesm: Removed /proc/%s\n", PROC_FILE_NAME);
}

module_init(memory_snapshot_init);
module_exit(memory_snapshot_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Secaida");
MODULE_DESCRIPTION("Syscall and proc interface for memory snapshot");
