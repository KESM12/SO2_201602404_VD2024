#ifndef _TRACK_SYSCALLS_H
#define _TRACK_SYSCALLS_H

#include <linux/types.h>
#include <linux/time.h>

// Definir los índices de las llamadas al sistema
#define SYS_OPEN_INDEX 0
#define SYS_WRITE_INDEX 1
#define SYS_READ_INDEX 2
#define SYS_FORK_INDEX 3
#define MAX_SYS_CALLS 4

struct syscall_usage {
    unsigned long count;
    struct timespec64 time_last_used;
} __attribute__((aligned(8)));

// Declaración de la función para que sea visible globalmente
void track_syscall(int syscall_id);

#endif /* _TRACK_SYSCALLS_H */