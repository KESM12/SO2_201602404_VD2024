#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

static unsigned long syscall_count[__NR_syscalls] = {0};

SYSCALL_DEFINE1(track_syscall_usage, int, syscall_id)
{
    if (syscall_id < 0 || syscall_id >= __NR_syscalls)
        return -EINVAL;

    syscall_count[syscall_id]++;
    return syscall_count[syscall_id];
}
