#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/oom.h>  // Incluye funciones relacionadas con el cálculo de OOM

struct process_mem_stats {
    unsigned long reserved_kb;    // Memoria reservada en KB
    unsigned long committed_kb;   // Memoria utilizada en KB
    unsigned long committed_pct;  // Porcentaje de memoria utilizada
    int oom_score;                // OOM Score
};

SYSCALL_DEFINE2(taro_ind_mem_stats, pid_t, pid, struct process_mem_stats __user *, stats) {
    struct task_struct *task;
    struct process_mem_stats local_stats;
    struct mm_struct *mm;
    unsigned long rss_kb;
    unsigned long total_memory_kb = totalram_pages() * PAGE_SIZE / 1024;

    // Buscar el proceso objetivo por su PID
    rcu_read_lock();
    task = find_task_by_vpid(pid);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH; // Proceso no encontrado
    }

    get_task_struct(task); // Asegurar que el proceso no sea eliminado durante el acceso
    rcu_read_unlock();

    mm = task->mm;
    if (!mm) {
        put_task_struct(task);
        return -EFAULT; // El proceso no tiene memoria asignada
    }

    // Calcular memoria reservada en KB
    local_stats.reserved_kb = (mm->total_vm * PAGE_SIZE) / 1024;

    // Calcular memoria utilizada (committed) en KB
    rss_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;
    local_stats.committed_kb = rss_kb;

    // Calcular el porcentaje de memoria utilizada
    if (local_stats.reserved_kb > 0) {
        local_stats.committed_pct = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
    } else {
        local_stats.committed_pct = 0; // Evitar división por cero
    }

    // Calcular el OOM Score
    unsigned long long oom_score = (rss_kb * 1000) / total_memory_kb; // RSS como proporción de memoria total, escalado a 1000
    int oom_adj = task->signal->oom_score_adj; // Ajuste OOM del proceso

    // Aplicar el ajuste de prioridad al OOM Score
    if (oom_adj > 0) {
        oom_score = oom_score * (oom_adj + 1000) / 1000;
    } else if (oom_adj < 0) {
        oom_score = oom_score / (-oom_adj + 1000) * 1000;
    }

    // Limitar el OOM Score a valores positivos
    local_stats.oom_score = (oom_score < 0) ? 0 : oom_score;

    put_task_struct(task);

    // Copiar resultados al espacio de usuario
    if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
        return -EFAULT;

    return 0; // Éxito
}



// #include <linux/kernel.h>
// #include <linux/syscalls.h>
// #include <linux/mm.h>
// #include <linux/sched.h>
// #include <linux/uaccess.h>

// struct process_mem_stats {
//     unsigned long reserved_kb;    // Memoria reservada en KB
//     unsigned long committed_kb;   // Memoria utilizada en KB
//     unsigned long committed_pct;  // Porcentaje de memoria utilizada
//     int oom_score;                // OOM Score
// };

// SYSCALL_DEFINE2(taro_ind_mem_stats, pid_t, pid, struct process_mem_stats __user *, stats) {
//     struct task_struct *task;
//     struct process_mem_stats local_stats;
//     struct mm_struct *mm;

//     // Buscar el proceso objetivo por su PID
//     rcu_read_lock();
//     task = find_task_by_vpid(pid);
//     if (!task) {
//         rcu_read_unlock();
//         return -ESRCH; // Proceso no encontrado
//     }

//     get_task_struct(task); // Asegurar que el proceso no sea eliminado durante el acceso
//     rcu_read_unlock();

//     mm = task->mm;
//     if (!mm) {
//         put_task_struct(task);
//         return -EFAULT; // El proceso no tiene memoria asignada
//     }

//     // Calcular memoria reservada en KB
//     local_stats.reserved_kb = (mm->total_vm * PAGE_SIZE) / 1024;

//     // Calcular memoria utilizada (committed) en KB
//     local_stats.committed_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;

//     // Calcular el porcentaje de memoria utilizada
//     if (local_stats.reserved_kb > 0) {
//         local_stats.committed_pct = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
//     } else {
//         local_stats.committed_pct = 0; // Evitar división por cero
//     }

//     // Calcular el OOM Score dinámicamente
//     unsigned long rss_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024; // RSS en KB
//     int oom_adj = task->signal->oom_score_adj; // Ajuste de OOM Score
//     unsigned long oom_score = rss_kb;

//     // Aplicar el ajuste de OOM Score
//     if (oom_adj > 0) {
//         oom_score = oom_score * (oom_adj + 1000) / 1000;
//     } else if (oom_adj < 0) {
//         oom_score = oom_score / (-oom_adj + 1000) * 1000;
//     }

//     // Limitar a 0 si el cálculo resultante es negativo
//     local_stats.oom_score = (oom_score < 0) ? 0 : oom_score;

//     put_task_struct(task);

//     // Copiar resultados al espacio de usuario
//     if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
//         return -EFAULT;

//     return 0; // Éxito
// }
