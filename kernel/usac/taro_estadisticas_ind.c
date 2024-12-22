#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/oom.h> // Incluye funciones relacionadas con el cálculo de OOM

struct process_mem_stats {
    unsigned long reserved_kb;    // Memoria reservada en KB
    unsigned long committed_kb;   // Memoria utilizada en KB
    unsigned long committed_pct;  // Porcentaje de memoria utilizada
    int oom_score;                // OOM Score
};

SYSCALL_DEFINE2(taro_ind_mem_stats, pid_t, pid, struct process_mem_stats __user *, stats) {
    struct task_struct *task;
    struct process_mem_stats local_stats = {0};
    struct mm_struct *mm;
    unsigned long total_pages;
    unsigned long badness;
    int oom_score_adj;

    if (pid <= 0 || !stats)
        return -EINVAL; // Argumentos inválidos

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

    // Calcular memoria reservada en KB
    local_stats.reserved_kb = (mm->total_vm * PAGE_SIZE) / 1024;

    // Calcular memoria utilizada (committed) en KB
    local_stats.committed_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;

    // Calcular el porcentaje de memoria utilizada
    if (local_stats.reserved_kb > 0) {
        local_stats.committed_pct = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
    } else {
        local_stats.committed_pct = 0; // Evitar división por cero
    }

    // Calcular "badness"
    total_pages = totalram_pages();
    badness = oom_badness(task, total_pages);

    // Obtener el ajuste de OOM Score
    oom_score_adj = task->signal->oom_score_adj;

    // Aplicar ajuste y escalar al rango de 0-1000
    badness = (badness * 1000) / total_pages;
    badness += oom_score_adj;

    // Asegurar que el puntaje esté dentro del rango [0, 1000]
    if (badness < 0)
        badness = 0;
    if (badness > 1000)
        badness = 1000;

    local_stats.oom_score = badness;

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
// #include <linux/oom.h>  // Incluye funciones relacionadas con el cálculo de OOM

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
//     unsigned long rss_kb;
//     unsigned long total_memory_kb = totalram_pages() * PAGE_SIZE / 1024;

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
//     rss_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;
//     local_stats.committed_kb = rss_kb;

//     // Calcular el porcentaje de memoria utilizada
//     if (local_stats.reserved_kb > 0) {
//         local_stats.committed_pct = (local_stats.committed_kb * 100) / local_stats.reserved_kb;
//     } else {
//         local_stats.committed_pct = 0; // Evitar división por cero
//     }

//     // Calcular el OOM Score
//     local_stats.oom_score = (rss_kb * 1000) / total_memory_kb; // Escalar en rango de 0-1000
//     local_stats.oom_score = local_stats.oom_score + task->signal->oom_score_adj;

//     // Asegurarse de que el OOM Score no sea negativo
//     if (local_stats.oom_score < 0) {
//         local_stats.oom_score = 0;
//     }

//     put_task_struct(task);

//     // Copiar resultados al espacio de usuario
//     if (copy_to_user(stats, &local_stats, sizeof(local_stats)))
//         return -EFAULT;

//     return 0; // Éxito
// }