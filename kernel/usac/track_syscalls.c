#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define MAX_SYS_CALLS 4

struct syscall_usage {
    unsigned long count;
    struct timespec64 time_last_used;
} __attribute__((aligned(8)));

static struct syscall_usage syscall_counters[MAX_SYS_CALLS];

// Prototipo de la función
long __sys_track_syscall_usage(struct syscall_usage __user *statistics);

// Función para rastrear llamadas al sistema
void track_syscall(int syscall_id)
{
    struct timespec64 now;

    if (syscall_id < 0 || syscall_id >= MAX_SYS_CALLS)
        return;

    syscall_counters[syscall_id].count++;
    ktime_get_real_ts64(&now);
    syscall_counters[syscall_id].time_last_used = now;
}
EXPORT_SYMBOL(track_syscall);

// Implementación de la syscall
long __sys_track_syscall_usage(struct syscall_usage __user *statistics)
{
    if (copy_to_user(statistics, syscall_counters, 
                     sizeof(struct syscall_usage) * MAX_SYS_CALLS))
        return -EFAULT;

    return 0;
}

// Wrapper para la syscall x86_64
asmlinkage long __x64_sys_track_syscall_usage(const struct pt_regs *regs)
{
    return __sys_track_syscall_usage((struct syscall_usage __user *)regs->di);
}
EXPORT_SYMBOL(sys_track_syscall_usage);

// Módulo de kernel (opcional, pero recomendado)
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Secaida");
MODULE_DESCRIPTION("Syscall tracking module");

// #include <linux/mm.h>
// #include <linux/gfp.h>
// #include <linux/uaccess.h>
// #include <linux/syscalls.h>
// #include <asm/unistd.h>
// #include <linux/time.h>
// #include <linux/ktime.h>

// #include "tracksyscalls.h"

// struct syscall_usage
// {
//     unsigned long count;
//     struct timespec64 time_last_used;
// } __attribute__((aligned(8)));

// #define MAX_SYS_CALLS 4

// static struct syscall_usage *syscall_counters;

// static int init_syscall_counters(void)
// {
//     syscall_counters = kzalloc(sizeof(struct syscall_usage) * MAX_SYS_CALLS, GFP_KERNEL);
//     return syscall_counters ? 0 : -ENOMEM;
// }

// void track_syscall(int syscall_id)
// {
//     struct timespec64 now;

//     if (!syscall_counters || syscall_id >= MAX_SYS_CALLS)
//     {
//         return;
//     }

//     syscall_counters[syscall_id].count++;
//     ktime_get_real_ts64(&now);
//     syscall_counters[syscall_id].time_last_used = now;
// }

// SYSCALL_DEFINE1(track_syscall_usage, struct syscall_usage __user *, statistics)
// {
//     if (!syscall_counters)
//     {
//         if (init_syscall_counters() != 0)
//         {
//             return -ENOMEM;
//         }
//     }
//     int resultadoCopia = copy_to_user(statistics, syscall_counters, sizeof(struct syscall_usage) * MAX_SYS_CALLS);
//     if (resultadoCopia)
//     {
//         return -EFAULT;
//     }
//     return 0;
// }
