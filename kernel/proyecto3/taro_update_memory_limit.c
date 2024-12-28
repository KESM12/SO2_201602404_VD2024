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

// Syscall para actualizar límites de memoria
SYSCALL_DEFINE2(taro_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limit_entry *entry;
    struct task_struct *task;

    if (process_pid < 0 || memory_limit < 0)
        return -EINVAL;

    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;

    rcu_read_lock();
    task = find_task_by_vpid(process_pid);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }
    rcu_read_unlock();

    mutex_lock(&memory_limit_mutex);

    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            if (task->mm && get_mm_rss(task->mm) > memory_limit) {
                mutex_unlock(&memory_limit_mutex);
                return -EEXCEED;
            }
            entry->limit = memory_limit;
            mutex_unlock(&memory_limit_mutex);
            return 0;
        }
    }

    mutex_unlock(&memory_limit_mutex);
    return -ENOTFOUND; // Not found in the list
}
