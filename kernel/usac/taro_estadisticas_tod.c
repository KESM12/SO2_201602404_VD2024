#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/rcupdate.h>

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