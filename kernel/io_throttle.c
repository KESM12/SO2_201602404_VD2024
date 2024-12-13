#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

struct io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long read_syscalls;
    unsigned long write_syscalls;
    unsigned long io_wait_time; 
};

SYSCALL_DEFINE2(get_io_throttle, pid_t, pid, struct io_stats __user *, stats)
{
    struct task_struct *task;
    struct io_stats io_stat;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }

    io_stat.bytes_read = task->ioac.read_bytes;
    io_stat.bytes_written = task->ioac.write_bytes;
    io_stat.read_syscalls = task->ioac.syscr;
    io_stat.write_syscalls = task->ioac.syscw;
    io_stat.io_wait_time = 0; 
    rcu_read_unlock();

    if (copy_to_user(stats, &io_stat, sizeof(io_stat)))
        return -EFAULT;

    return 0;
}
