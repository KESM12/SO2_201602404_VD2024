#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define MAX_PROCESSES 1024

struct process_mem_stats {
    pid_t pid;                    // PID del proceso
    unsigned long reserved_kb;    // Memoria reservada en KB
    unsigned long committed_kb;   // Memoria utilizada en KB
    unsigned long committed_pct;  // Porcentaje utilizado
    int oom_score;                // OOM Score
};

void print_table_header() {
    printf("+----------+---------------+---------------+----------+-----------+\n");
    printf("| %-8s | %-13s | %-13s | %-8s | %-8s |\n", 
           "PID", "Reserved (KB)", "Committed(KB)", "Pct (%)", "OOM Score");
    printf("+----------+---------------+---------------+----------+-----------+\n");
}

void print_process_stats(struct process_mem_stats *stats) {
    printf("| %-8d | %-13lu | %-13lu | %-8lu | %-8d  |\n",
           stats->pid,
           stats->reserved_kb,
           stats->committed_kb,
           stats->committed_pct,
           stats->oom_score);
    printf("+----------+---------------+---------------+----------+-----------+\n");
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pid|0>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    struct process_mem_stats stats[MAX_PROCESSES];
    size_t buffer_size = sizeof(stats);

    ssize_t ret = syscall(552, pid, stats, buffer_size);

    if (ret < 0) {
        perror("Error al obtener estadísticas de memoria");
        return errno;
    }

    size_t num_entries = ret / sizeof(struct process_mem_stats);

    // Mostrar resultados
    print_table_header();
    for (size_t i = 0; i < num_entries; i++) {
        print_process_stats(&stats[i]);
    }

    return 0;
}
