#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/cred.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/mutex.h>

// Error codes
define ESRCH 3   // No such process
define EINVAL 22 // Invalid argument
define EPERM  1  // Operation not permitted
define ENOMEM 12 // Out of memory
#define EEXCEED 100 // Process already exceeds memory limit
#define EDUPLICATE 101 // Process is already in the list

// Define structure for memory limits
struct memory_limit_entry {
    pid_t pid;
    long limit;
    struct list_head list;
};

static LIST_HEAD(memory_limit_list); // Global list to store memory limits
static DEFINE_MUTEX(memory_limit_mutex); // Mutex to protect the list

SYSCALL_DEFINE2(taro_add_memory_limit, pid_t, pid, long, limit) {
    struct memory_limit_entry *entry;
    struct task_struct *task;
    struct memory_limit_entry *tmp;

    // Validate PID and limit
    if (pid < 0 || limit < 0)
        return -EINVAL;

    // Check if the current user is root (sudoer)
    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;

    // Find the task by PID
    rcu_read_lock();
    task = find_task_by_vpid(pid);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }
    rcu_read_unlock();

    mutex_lock(&memory_limit_mutex);

    // Check if the process is already in the list
    list_for_each_entry(tmp, &memory_limit_list, list) {
        if (tmp->pid == pid) {
            mutex_unlock(&memory_limit_mutex);
            return -EDUPLICATE;
        }
    }

    // Check if the process already exceeds the memory limit
    if (task->mm && get_mm_rss(task->mm) > limit) {
        mutex_unlock(&memory_limit_mutex);
        return -EEXCEED;
    }

    // Allocate memory for the new entry
    entry = kmalloc(sizeof(struct memory_limit_entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_mutex);
        return -ENOMEM;
    }

    // Initialize the entry
    entry->pid = pid;
    entry->limit = limit;
    INIT_LIST_HEAD(&entry->list);

    // Add to the global list
    list_add(&entry->list, &memory_limit_list);

    mutex_unlock(&memory_limit_mutex);

    return 0; // Success
}
