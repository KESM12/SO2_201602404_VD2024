#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/mutex.h>

struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
    struct memory_limitation *next;
};

static struct memory_limitation *memory_limit_head = NULL;
static DEFINE_MUTEX(memory_limit_mutex);

SYSCALL_DEFINE3(taro_get_memory_limits, struct memory_limitation*, u_processes_buffer, size_t, max_entries, int*, processes_returned) {
    struct memory_limitation *current_node = NULL;
    struct memory_limitation *k_buffer;
    int count = 0;

    // Validar parámetros de entrada
    if (!u_processes_buffer || !processes_returned || max_entries <= 0) {
        pr_info("taro_get_memory_limits: Parámetros inválidos.\n");
        return -EINVAL;
    }

    // Intentar asignar memoria para el buffer en espacio de kernel
    k_buffer = kmalloc_array(max_entries, sizeof(struct memory_limitation), GFP_KERNEL);
    if (!k_buffer) {
        pr_info("taro_get_memory_limits: Sin memoria para el buffer de kernel.\n");
        return -ENOMEM;
    }

    mutex_lock(&memory_limit_mutex);

    // Iterar sobre la lista y copiar las entradas al buffer
    current_node = memory_limit_head;
    while (current_node != NULL && count < max_entries) {
        k_buffer[count].pid = current_node->pid;
        k_buffer[count].memory_limit = current_node->memory_limit;
        count++;
        current_node = current_node->next;
    }

    mutex_unlock(&memory_limit_mutex);

    // Copiar el buffer del kernel al espacio de usuario
    if (copy_to_user(u_processes_buffer, k_buffer, count * sizeof(struct memory_limitation))) {
        pr_info("taro_get_memory_limits: Error al copiar al espacio de usuario.\n");
        kfree(k_buffer);
        return -EFAULT;
    }

    // Copiar el número de procesos escritos al espacio de usuario
    if (copy_to_user(processes_returned, &count, sizeof(int))) {
        pr_info("taro_get_memory_limits: Error al copiar el conteo al espacio de usuario.\n");
        kfree(k_buffer);
        return -EFAULT;
    }

    pr_info("taro_get_memory_limits: %d procesos escritos al buffer.\n", count);

    kfree(k_buffer);
    return 0;
}
