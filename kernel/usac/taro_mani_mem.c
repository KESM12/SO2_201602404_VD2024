#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/resource.h>
#include <linux/sched.h>

// Definición de la estructura de la lista enlazada
typedef struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
    struct list_head list; 
} memory_limitation_t;

// Cabeza de la lista global
static LIST_HEAD(memory_limit_list);
static DEFINE_MUTEX(memory_limit_mutex);

// Syscall 1: Agregar un límite de memoria
SYSCALL_DEFINE2(taro_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    memory_limitation_t *entry, *temp;
    struct task_struct *task;
    struct rlimit rl;

    // Validar parámetros
    if (process_pid < 0 || memory_limit <= 0) {
        return -EINVAL;
    }

    // Buscar el proceso por su PID
    task = find_task_by_vpid(process_pid);
    if (!task) {
        return -ESRCH; // Proceso no encontrado
    }

    mutex_lock(&memory_limit_mutex);

    // Verificar si el proceso ya está en la lista
    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            mutex_unlock(&memory_limit_mutex);
            return -101; // El proceso ya está en la lista
        }
    }

    // Crear un nuevo nodo
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_mutex);
        return -ENOMEM;
    }

    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    // Agregar el nodo al final de la lista
    list_add_tail(&entry->list, &memory_limit_list);

    // Configurar el límite de memoria utilizando rlimit
    rl.rlim_cur = memory_limit;
    rl.rlim_max = memory_limit;

    get_task_struct(task);

    if (security_task_setrlimit(task, RLIMIT_AS, &rl)) {
        kfree(entry); // Liberar memoria si falla
        list_del(&entry->list); // Eliminar de la lista si fue agregado
        put_task_struct(task);
        mutex_unlock(&memory_limit_mutex);
        return -EPERM; // Permisos insuficientes
    }

    task->signal->rlim[RLIMIT_AS] = rl;

    put_task_struct(task);
    mutex_unlock(&memory_limit_mutex);

    return 0;
}

// Syscall 2: Obtener los procesos limitados
SYSCALL_DEFINE3(taro_get_memory_limits, struct memory_limitation __user *, u_processes_buffer, size_t, max_entries, int __user *, processes_returned) {
    memory_limitation_t *temp;
    struct {
        pid_t pid;
        size_t memory_limit;
    } *k_buffer; 
    int count = 0;

    if (!u_processes_buffer || !processes_returned || max_entries <= 0) {
        return -EINVAL;
    }

    k_buffer = kmalloc_array(max_entries, sizeof(*k_buffer), GFP_KERNEL);
    if (!k_buffer) {
        return -ENOMEM;
    }

    mutex_lock(&memory_limit_mutex);

    list_for_each_entry(temp, &memory_limit_list, list) {
        if (count >= max_entries) {
            break;
        }

        k_buffer[count].pid = temp->pid;
        k_buffer[count].memory_limit = temp->memory_limit;

        pr_info("taro_get_memory_limits: Nodo %d - PID=%d, Limit=%zu\n", count, temp->pid, temp->memory_limit);
        count++;
    }

    mutex_unlock(&memory_limit_mutex);

    if (copy_to_user(u_processes_buffer, k_buffer, count * sizeof(*k_buffer))) {
        kfree(k_buffer);
        return -EFAULT;
    }

    if (copy_to_user(processes_returned, &count, sizeof(int))) {
        kfree(k_buffer);
        return -EFAULT;
    }

    kfree(k_buffer);
    return 0;
}

// Syscall 3: Actualizar el límite de memoria de un proceso
SYSCALL_DEFINE2(taro_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    memory_limitation_t *temp;

    if (process_pid < 0 || memory_limit <= 0) {
        return -EINVAL;
    }

    mutex_lock(&memory_limit_mutex);

    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            temp->memory_limit = memory_limit;
            mutex_unlock(&memory_limit_mutex);
            return 0;
        }
    }

    mutex_unlock(&memory_limit_mutex);
    return -102; // Proceso no encontrado
}

// Syscall 4: Eliminar el límite de memoria de un proceso
SYSCALL_DEFINE1(taro_remove_memory_limit, pid_t, process_pid) {
    memory_limitation_t *temp;

    if (process_pid < 0) {
        return -EINVAL;
    }

    mutex_lock(&memory_limit_mutex);

    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            list_del(&temp->list);
            kfree(temp);
            mutex_unlock(&memory_limit_mutex);
            return 0;
        }
    }

    mutex_unlock(&memory_limit_mutex);
    return -102; // Proceso no encontrado
}
