#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/cred.h>

// Prototipos de funciones
long validate_memory_limit(pid_t process_pid, size_t memory_limit);
long add_to_memory_limit_list(pid_t process_pid, size_t memory_limit);

struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
    struct list_head list;
};

static LIST_HEAD(memory_limit_list);
static DEFINE_MUTEX(memory_limit_mutex);

long validate_memory_limit(pid_t process_pid, size_t memory_limit) {
    struct task_struct *task;

    // Validar PID y límite de memoria no negativos
    if (process_pid < 0 || memory_limit < 0) {
        pr_info("taro_add_memory_limit: PID %d o límite de memoria %zu inválido.\n", process_pid, memory_limit);
        return -EINVAL;
    }

    // Verificar si el proceso con el PID especificado existe
    task = find_task_by_vpid(process_pid);
    if (!task) {
        pr_info("taro_add_memory_limit: PID %d no encontrado.\n", process_pid);
        return -ESRCH;
    }

    return 0;
}

long add_to_memory_limit_list(pid_t process_pid, size_t memory_limit) {
    struct memory_limitation *entry;
    struct memory_limitation *temp;
    struct task_struct *task;
    long result;

    // Verificar permisos de superusuario
    if (!capable(CAP_SYS_ADMIN)) {
        pr_info("taro_add_memory_limit: Permiso denegado para PID %d.\n", process_pid);
        return -EPERM;
    }

    // Validar PID y límite de memoria
    result = validate_memory_limit(process_pid, memory_limit);
    if (result != 0) {
        return result;
    }

    task = find_task_by_vpid(process_pid);

    mutex_lock(&memory_limit_mutex);

    // Verificar si el proceso ya está en la lista
    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            mutex_unlock(&memory_limit_mutex);
            pr_info("taro_add_memory_limit: PID %d ya está en la lista.\n", process_pid);
            return -101; // Proceso ya en la lista
        }
    }

    // Intentar asignar memoria para la nueva entrada
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_mutex);
        pr_info("taro_add_memory_limit: Sin memoria para PID %d.\n", process_pid);
        return -ENOMEM; // Sin memoria para agregar el nodo
    }

    // Verificar si el proceso ya excede el límite de memoria
    if (task->mm && task->mm->total_vm > memory_limit) {
        kfree(entry);
        mutex_unlock(&memory_limit_mutex);
        pr_info("taro_add_memory_limit: PID %d ya excede el límite de memoria.\n", process_pid);
        return -100; // El proceso ya excede el límite
    }

    // Agregar el proceso a la lista
    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    list_add_tail(&entry->list, &memory_limit_list);

    mutex_unlock(&memory_limit_mutex);

    pr_info("taro_add_memory_limit: PID %d agregado con límite %zu bytes.\n", process_pid, memory_limit);
    return 0;
}

SYSCALL_DEFINE2(taro_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    long result = add_to_memory_limit_list(process_pid, memory_limit);

    if (result < 0) {
        set_current_state(TASK_INTERRUPTIBLE);
        return result;
    }

    return 0;
}
