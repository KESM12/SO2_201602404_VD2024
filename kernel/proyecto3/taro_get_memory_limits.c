#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/list.h>

// Estructura para límites de memoria que será compartida con el espacio de usuario
struct memory_limitation {
    pid_t pid;
    long limit;
};

// Estructura interna del kernel ya definida
extern struct list_head memory_limit_list; // Lista global
extern struct mutex memory_limit_mutex;    // Mutex global

SYSCALL_DEFINE3(taro_get_memory_limits, struct memory_limitation __user *, u_processes_buffer, size_t, max_entries, int __user *, processes_returned) {
    struct memory_limit_entry *entry;
    struct memory_limitation *k_buffer; // Buffer kernel
    size_t count = 0;
    int ret;

    // Validar el puntero del espacio de usuario
    if (!u_processes_buffer || !processes_returned)
        return -EINVAL;

    // Validar que max_entries sea un valor positivo
    if (max_entries <= 0)
        return -EINVAL;

    // Asignar un buffer en espacio kernel
    k_buffer = kmalloc_array(max_entries, sizeof(struct memory_limitation), GFP_KERNEL);
    if (!k_buffer)
        return -ENOMEM;

    mutex_lock(&memory_limit_mutex);

    // Recorrer la lista y llenar el buffer kernel hasta el máximo permitido
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (count >= max_entries)
            break;

        k_buffer[count].pid = entry->pid;
        k_buffer[count].limit = entry->limit;
        count++;
    }

    mutex_unlock(&memory_limit_mutex);

    // Copiar el buffer kernel al espacio de usuario
    ret = copy_to_user(u_processes_buffer, k_buffer, count * sizeof(struct memory_limitation));
    if (ret) {
        kfree(k_buffer);
        return -EFAULT;
    }

    // Copiar la cantidad de procesos escritos al puntero de usuario
    ret = put_user(count, processes_returned);
    if (ret) {
        kfree(k_buffer);
        return -EFAULT;
    }

    // Liberar el buffer kernel
    kfree(k_buffer);

    return 0; // Éxito
}
