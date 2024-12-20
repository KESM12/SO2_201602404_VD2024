#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

struct system_mem_stats {
    unsigned long total_reserved_mb;  // Memoria total reservada en MB
    unsigned long total_committed_mb; // Memoria total utilizada en MB
};

int main() {
    struct system_mem_stats stats;

    if (syscall(553, &stats) == 0) {
        printf("Estadísticas de memoria del sistema:\n");
        printf("  Memoria total reservada: %lu MB\n", stats.total_reserved_mb);
        printf("  Memoria total utilizada: %lu MB\n", stats.total_committed_mb);
    } else {
        perror("Error al obtener estadísticas de memoria del sistema");
        return errno;
    }

    return 0;
}
