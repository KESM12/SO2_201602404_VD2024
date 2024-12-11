#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
};

SYSCALL_DEFINE1(capture_memory_snapshot, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot ksnapshot;
    struct sysinfo si;

    si_meminfo(&si);

    ksnapshot.total_memory = si.totalram << (PAGE_SHIFT - 10); // KB
    ksnapshot.free_memory = si.freeram << (PAGE_SHIFT - 10);  // KB
    ksnapshot.active_pages = global_node_page_state(NR_ACTIVE_FILE);
    ksnapshot.inactive_pages = global_node_page_state(NR_INACTIVE_FILE);

    if (copy_to_user(snapshot, &ksnapshot, sizeof(ksnapshot)))
        return -EFAULT;

    return 0;
}
