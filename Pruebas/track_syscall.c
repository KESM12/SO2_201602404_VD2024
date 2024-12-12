#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define SYSCALL_TRACK_SYSCALL_USAGE 549

int main() {
    int syscall_id = __NR_write;  // Por ejemplo, rastrear la syscall 'write'
    long count;

    count = syscall(SYSCALL_TRACK_SYSCALL_USAGE, syscall_id);
    if (count < 0) {
        perror("Error en track_syscall_usage");
        return errno;
    }

    printf("Syscall %d ha sido llamada %ld veces\n", syscall_id, count);
    return 0;
}
