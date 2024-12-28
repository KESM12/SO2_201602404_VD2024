#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/pid.h>

// Estructura para los límites de memoria
struct memory_limit_entry {
    pid_t pid;
    long limit;
    struct list_head list;
};

static LIST_HEAD(memory_limit_list); // Lista global
static DEFINE_MUTEX(memory_limit_mutex); // Mutex global

#define ESRCH 3      // No such process
#define EINVAL 22    // Invalid argument
#define EPERM 1      // Operation not permitted
#define EEXCEED 100  // Exceeds memory limit
#define ENOTFOUND 102 // Not found in the list

// Syscall para remover límites de memoria
SYSCALL_DEFINE1(taro_remove_memory_limit, pid_t, process_pid) {
    struct memory_limit_entry *entry, *tmp;

    if (process_pid < 0)
        return -EINVAL;

    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;

    mutex_lock(&memory_limit_mutex);

    list_for_each_entry_safe(entry, tmp, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            list_del(&entry->list);
            kfree(entry);
            mutex_unlock(&memory_limit_mutex);
            return 0; // Success
        }
    }

    mutex_unlock(&memory_limit_mutex);
    return -ENOTFOUND; // Not found in the list
}
