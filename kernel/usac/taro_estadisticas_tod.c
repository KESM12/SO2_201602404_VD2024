// #include <linux/kernel.h>
// #include <linux/syscalls.h>
// #include <linux/mm.h>
// #include <linux/sched.h>
// #include <linux/uaccess.h>
// #include <linux/oom.h> // Funciones relacionadas con OOM

// struct total_mem_stats {
//     unsigned long total_reserved_kb; // Memoria total reservada en KB
//     unsigned long total_committed_kb; // Memoria total utilizada en KB
// };

// SYSCALL_DEFINE1(taro_tod_mem_stats, struct total_mem_stats __user *, stats) {
//     struct task_struct *task;
//     struct total_mem_stats local_stats = {0}; // Inicializa la estructura con ceros
//     unsigned long total_reserved_kb = 0;
//     unsigned long total_committed_kb = 0;
//     unsigned long rss_kb;

//     // Iterar sobre todos los procesos
//     for_each_process(task) {
//         struct mm_struct *mm = task->mm;

//         if (!mm) {
//             continue; // El proceso no tiene espacio de memoria asignado
//         }

//         // Calcular memoria reservada y utilizada para este proceso
//         total_reserved_kb += (mm->total_vm * PAGE_SIZE) / 1024;
//         rss_kb = (get_mm_rss(mm) * PAGE_SIZE) / 1024;
//         total_committed_kb += rss_kb;
//     }

//     // Guardar los totales calculados en la estructura local
//     local_stats.total_reserved_kb = total_reserved_kb;
//     local_stats.total_committed_kb = total_committed_kb;

//     // Copiar resultados al espacio de usuario
//     if (copy_to_user(stats, &local_stats, sizeof(local_stats))) {
//         return -EFAULT;
//     }

//     return 0; // Éxito
// }



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
