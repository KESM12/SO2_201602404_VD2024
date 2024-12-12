#include <stdio.h>
#include <stdlib.h> // Agrega este encabezado para atoi
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define SYSCALL_GET_IO_THROTTLE 550 

struct io_stats {
    unsigned long bytes_read;
    unsigned long bytes_written;
    unsigned long read_syscalls;
    unsigned long write_syscalls;
    unsigned long io_wait_time;  // Placeholder
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return -1;
    }

    pid_t pid = atoi(argv[1]); // atoi convierte string a entero
    struct io_stats stats;

    if (syscall(SYSCALL_GET_IO_THROTTLE, pid, &stats) == -1) {
        perror("Error en get_io_throttle");
        return errno;
    }

    printf("Estadísticas de I/O para PID %d:\n", pid);
    printf("Bytes leídos: %lu\n", stats.bytes_read);
    printf("Bytes escritos: %lu\n", stats.bytes_written);
    printf("Syscalls de lectura: %lu\n", stats.read_syscalls);
    printf("Syscalls de escritura: %lu\n", stats.write_syscalls);
    printf("Tiempo de espera de I/O: %lu (placeholder)\n", stats.io_wait_time);

    return 0;
}