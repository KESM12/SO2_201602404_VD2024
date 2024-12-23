#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/oom.h> 

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