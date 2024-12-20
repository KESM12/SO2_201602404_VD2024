#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

struct process_mem_stats {
    unsigned long reserved_kb;    // Memoria reservada en KB
    unsigned long committed_kb;   // Memoria utilizada en KB
    unsigned long committed_pct;  // Porcentaje de memoria utilizada
    int oom_score;                // OOM Score
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    struct process_mem_stats stats;

    if (syscall(552, pid, &stats) == 0) {
        printf("Estadísticas de memoria para PID %d:\n", pid);
        printf("  Memoria reservada: %lu KB\n", stats.reserved_kb);
        printf("  Memoria utilizada: %lu KB\n", stats.committed_kb);
        printf("  Porcentaje utilizado: %lu%%\n", stats.committed_pct);
        printf("  OOM Score: %d\n", stats.oom_score);
    } else {
        perror("Error al obtener estadísticas de memoria");
        return errno;
    }

    return 0;
}
