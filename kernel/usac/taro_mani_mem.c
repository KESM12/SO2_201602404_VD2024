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

SYSCALL_DEFINE2(taro_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    memory_limitation_t *entry, *temp;
    struct task_struct *task;
    struct rlimit rl;

    // Validar parámetros
    if (process_pid < 0 || memory_limit <= 0) {
        pr_info("taro_add_memory_limit: PID inválido (%d) o límite inválido (%zu).\n", process_pid, memory_limit);
        return -EINVAL;
    }

    // Buscar el proceso por su PID
    task = find_task_by_vpid(process_pid);
    if (!task) {
        pr_info("taro_add_memory_limit: Proceso no encontrado (PID=%d).\n", process_pid);
        return -ESRCH; // Proceso no encontrado
    }

    mutex_lock(&memory_limit_mutex);

    // Verificar si el proceso ya está en la lista
    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            pr_info("taro_add_memory_limit: PID %d ya está en la lista.\n", process_pid);
            mutex_unlock(&memory_limit_mutex);
            return -101; // El proceso ya está en la lista
        }
    }

    // Crear un nuevo nodo
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        pr_info("taro_add_memory_limit: Sin memoria para crear nodo (PID=%d).\n", process_pid);
        mutex_unlock(&memory_limit_mutex);
        return -ENOMEM;
    }

    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    // Agregar el nodo al final de la lista
    list_add_tail(&entry->list, &memory_limit_list);
    pr_info("taro_add_memory_limit: Nodo agregado (PID=%d, Limit=%zu bytes).\n", process_pid, memory_limit);

    // Configurar el límite de memoria utilizando rlimit
    rl.rlim_cur = memory_limit;
    rl.rlim_max = memory_limit;

    get_task_struct(task);

    if (security_task_setrlimit(task, RLIMIT_AS, &rl)) {
        pr_info("taro_add_memory_limit: Fallo al aplicar RLIMIT_AS (PID=%d).\n", process_pid);
        kfree(entry);
        list_del(&entry->list);
        put_task_struct(task);
        mutex_unlock(&memory_limit_mutex);
        return -EPERM; // Permisos insuficientes
    }

    task->signal->rlim[RLIMIT_AS] = rl;

    pr_info("taro_add_memory_limit: RLIMIT_AS aplicado correctamente (PID=%d, Limit=%zu bytes).\n", process_pid, memory_limit);

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
    pr_info("taro_get_memory_limits: Nodo %d - PID=%d, Limit=%zu\n", count, temp->pid, temp->memory_limit);
    return 0;
}

// Syscall 3: Actualizar el límite de memoria de un proceso
SYSCALL_DEFINE2(taro_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    memory_limitation_t *temp;
    struct task_struct *task;
    struct rlimit rl;

    // Validar parámetros
    if (process_pid < 0 || memory_limit <= 0) {
        pr_info("Syscall taro_update_memory_limit called1: PID=%d, New Limit=%zu bytes\n", process_pid, memory_limit);
        return -EINVAL;
    }

    // Buscar el proceso por su PID
    task = find_task_by_vpid(process_pid);
    if (!task) {
        return -ESRCH; // Proceso no encontrado
    }

    mutex_lock(&memory_limit_mutex);
    pr_info("Syscall taro_update_memory_limit called2: PID=%d, New Limit=%zu bytes\n", process_pid, memory_limit);
    // Buscar el proceso en la lista
    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            // Verificar si el proceso ya excede el nuevo límite
            if (task->mm && task->mm->total_vm > (memory_limit >> PAGE_SHIFT)) {
                mutex_unlock(&memory_limit_mutex);
                return -100; // El proceso ya excede el nuevo límite
            }

            // Actualizar el límite en la lista
            temp->memory_limit = memory_limit;

            // Configurar el nuevo límite de memoria utilizando rlimit
            rl.rlim_cur = memory_limit;
            rl.rlim_max = memory_limit;

            get_task_struct(task);

            if (security_task_setrlimit(task, RLIMIT_AS, &rl)) {
                put_task_struct(task);
                mutex_unlock(&memory_limit_mutex);
                return -EPERM; // Permisos insuficientes
            }

            task->signal->rlim[RLIMIT_AS] = rl;

            put_task_struct(task);
            mutex_unlock(&memory_limit_mutex);
            pr_info("Syscall taro_update_memory_limit called: PID=%d, New Limit=%zu bytes\n", process_pid, memory_limit);


            return 0; // Límite actualizado exitosamente
        }
    }
    mutex_unlock(&memory_limit_mutex);
    
    return -102; // Proceso no encontrado en la lista
}


// Syscall 4: Eliminar el límite de memoria de un proceso
SYSCALL_DEFINE1(taro_remove_memory_limit, pid_t, process_pid) {
    memory_limitation_t *temp;

    // Validar PID
    if (process_pid <= 0) {
        pr_info("taro_remove_memory_limit1: PID inválido (%d).\n", process_pid);
        return -EINVAL;
    }

    // Adquirir el mutex
    mutex_lock(&memory_limit_mutex);

    // Buscar el proceso en la lista
    list_for_each_entry(temp, &memory_limit_list, list) {
        if (temp->pid == process_pid) {
            // Encontrado: Eliminar nodo de la lista
            list_del(&temp->list);
            kfree(temp);
            mutex_unlock(&memory_limit_mutex);
            pr_info("taro_remove_memory_limit2: PID %d eliminado correctamente.\n", process_pid);
            return 0;
        }
    }

    // Nodo no encontrado
    mutex_unlock(&memory_limit_mutex);
    pr_info("taro_remove_memory_limit3: PID %d no encontrado en la lista.\n", process_pid);
    return -102; // Proceso no encontrado
}