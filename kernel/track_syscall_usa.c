#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>

// Estructura para almacenar las estadísticas de las llamadas al sistema
struct syscall_stats {
    unsigned long open_count;
    unsigned long read_count;
    unsigned long write_count;
    unsigned long fork_count;
};

// Instancia global de las estadísticas de las llamadas al sistema
static struct syscall_stats syscalls_usage = {0, 0, 0, 0};

// Función que incrementa el contador de llamadas al sistema
static inline void increment_syscall_count(int syscall_id) {
    switch (syscall_id) {
        case __NR_open:
            syscalls_usage.open_count++;
            break;
        case __NR_read:
            syscalls_usage.read_count++;
            break;
        case __NR_write:
            syscalls_usage.write_count++;
            break;
        case __NR_fork:
            syscalls_usage.fork_count++;
            break;
        default:
            break;
    }
}

// Función de monitoreo para las llamadas al sistema
asmlinkage long sys_open(const char __user *filename, int flags, umode_t mode) {
    long ret = original_sys_open(filename, flags, mode); // Llamada original
    increment_syscall_count(__NR_open);
    return ret;
}

asmlinkage ssize_t sys_read(unsigned int fd, char __user *buf, size_t count) {
    ssize_t ret = original_sys_read(fd, buf, count); // Llamada original
    increment_syscall_count(__NR_read);
    return ret;
}

asmlinkage ssize_t sys_write(unsigned int fd, const char __user *buf, size_t count) {
    ssize_t ret = original_sys_write(fd, buf, count); // Llamada original
    increment_syscall_count(__NR_write);
    return ret;
}

asmlinkage pid_t sys_fork(void) {
    pid_t ret = original_sys_fork(); // Llamada original
    increment_syscall_count(__NR_fork);
    return ret;
}

// Función para capturar las estadísticas de las llamadas al sistema
SYSCALL_DEFINE1(track_syscall_usage, struct syscall_stats __user *, stats) {
    if (copy_to_user(stats, &syscalls_usage, sizeof(syscalls_usage)))
        return -EFAULT;

    return 0;
}

// Inicialización del módulo
static int __init syscall_monitor_init(void) {
    printk(KERN_INFO "Cargando módulo de monitoreo de llamadas al sistema\n");

    // Aquí se debe guardar la dirección de la llamada original a la tabla de llamadas al sistema
    original_sys_open = (void *)sys_call_table[__NR_open];
    original_sys_read = (void *)sys_call_table[__NR_read];
    original_sys_write = (void *)sys_call_table[__NR_write];
    original_sys_fork = (void *)sys_call_table[__NR_fork];

    // Reemplazar las llamadas al sistema con nuestras versiones personalizadas
    sys_call_table[__NR_open] = (unsigned long)sys_open;
    sys_call_table[__NR_read] = (unsigned long)sys_read;
    sys_call_table[__NR_write] = (unsigned long)sys_write;
    sys_call_table[__NR_fork] = (unsigned long)sys_fork;

    return 0;
}

// Función de limpieza del módulo
static void __exit syscall_monitor_exit(void) {
    printk(KERN_INFO "Desmontando módulo de monitoreo de llamadas al sistema\n");

    // Restaurar las llamadas al sistema originales
    sys_call_table[__NR_open] = (unsigned long)original_sys_open;
    sys_call_table[__NR_read] = (unsigned long)original_sys_read;
    sys_call_table[__NR_write] = (unsigned long)original_sys_write;
    sys_call_table[__NR_fork] = (unsigned long)original_sys_fork;
}

module_init(syscall_monitor_init);
module_exit(syscall_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tu Nombre");
MODULE_DESCRIPTION("Módulo para rastrear las llamadas al sistema");
