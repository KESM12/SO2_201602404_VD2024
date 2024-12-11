#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

// Estructura para almacenar los datos de la memoria
struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
};

// Variable global para almacenar el último snapshot
static struct mem_snapshot last_snapshot;

// Función para manejar la lectura del archivo en /proc
static int memory_snapshot_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Total Memory: %lu KB\n", last_snapshot.total_memory);
    seq_printf(m, "Free Memory: %lu KB\n", last_snapshot.free_memory);
    seq_printf(m, "Active Pages: %lu\n", last_snapshot.active_pages);
    seq_printf(m, "Inactive Pages: %lu\n", last_snapshot.inactive_pages);
    return 0;
}

static int memory_snapshot_open(struct inode *inode, struct file *file)
{
    return single_open(file, memory_snapshot_show, NULL);
}

static const struct proc_ops memory_snapshot_fops = {
    .proc_open    = memory_snapshot_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

// Syscall para capturar el estado de la memoria
SYSCALL_DEFINE1(capture_memory_snapshot, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot ksnapshot;
    struct sysinfo si;

    // Obtén la información de memoria
    si_meminfo(&si);
    ksnapshot.total_memory = si.totalram << (PAGE_SHIFT - 10); // KB
    ksnapshot.free_memory = si.freeram << (PAGE_SHIFT - 10);  // KB
    ksnapshot.active_pages = global_node_page_state(NR_ACTIVE_FILE);
    ksnapshot.inactive_pages = global_node_page_state(NR_INACTIVE_FILE);

    // Copia al espacio de usuario
    if (copy_to_user(snapshot, &ksnapshot, sizeof(ksnapshot)))
        return -EFAULT;

    // Actualiza el último snapshot
    last_snapshot = ksnapshot;

    return 0;
}

// Funciones de inicialización y limpieza
static int __init memory_snapshot_init(void)
{
    // Crear el archivo en /proc
    proc_create("memory_snapshot_kesm", 0444, NULL, &memory_snapshot_fops);
    pr_info("memory_snapshot_kesm: Created /proc/memory_snapshot_kesm\n");
    return 0;
}

static void __exit memory_snapshot_exit(void)
{
    // Eliminar el archivo de /proc
    remove_proc_entry("memory_snapshot_kesm", NULL);
    pr_info("memory_snapshot_kesm: Removed /proc/memory_snapshot_kesm\n");
}

module_init(memory_snapshot_init);
module_exit(memory_snapshot_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Secaida");
MODULE_DESCRIPTION("Syscall and proc interface for memory snapshot");



// #include <linux/kernel.h>
// #include <linux/syscalls.h>
// #include <linux/mm.h>
// #include <linux/sched.h>
// #include <linux/uaccess.h>

// struct mem_snapshot {
//     unsigned long total_memory;
//     unsigned long free_memory;
//     unsigned long active_pages;
//     unsigned long inactive_pages;
// };

// SYSCALL_DEFINE1(capture_memory_snapshot, struct mem_snapshot __user *, snapshot)
// {
//     struct mem_snapshot ksnapshot;
//     struct sysinfo si;

//     si_meminfo(&si);

//     ksnapshot.total_memory = si.totalram << (PAGE_SHIFT - 10); // KB
//     ksnapshot.free_memory = si.freeram << (PAGE_SHIFT - 10);  // KB
//     ksnapshot.active_pages = global_node_page_state(NR_ACTIVE_FILE);
//     ksnapshot.inactive_pages = global_node_page_state(NR_INACTIVE_FILE);

//     if (copy_to_user(snapshot, &ksnapshot, sizeof(ksnapshot)))
//         return -EFAULT;

//     return 0;
// }
