#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

struct system_mem_stats {
    unsigned long total_reserved_mb;  // Memoria total reservada en MB
    unsigned long total_committed_mb; // Memoria total utilizada en MB
};

SYSCALL_DEFINE1(taro_tod_mem_stats, struct system_mem_stats __user *, stats) {
    struct task_struct *task;
    struct mm_struct *mm;
    struct system_mem_stats local_stats = {0, 0};

    // Iterar sobre todos los procesos
    for_each_process(task) {
        mm = task->mm;
        if (!mm)
            continue; // Saltar procesos sin mm_struct

        // Acumular memoria reservada
        local_stats.total_reserved_mb += (mm->total_vm * PAGE_SIZE) / (1024 * 1024);

        // Acumular memoria utilizada
        local_stats.total_committed_mb += (get_mm_rss(mm) * PAGE_SIZE) / (1024 * 1024);
    }

    // Copiar resultados al espacio de usuario
    if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
        return -EFAULT;

    return 0; // Éxito
}
