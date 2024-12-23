#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#define SYS_taro_tod_mem_totals 553

// Estructura para los totales de memoria
struct total_mem_stats {
    unsigned long total_reserved_mb;   // Total de memoria reservada en MB
    unsigned long total_committed_mb;  // Total de memoria comprometida en MB
};

int main() {
    struct total_mem_stats totals;

    // Llamar a la syscall
    int result = syscall(SYS_taro_tod_mem_totals, &totals);

    if (result < 0) {
        perror("Error al invocar la syscall");
        return EXIT_FAILURE;
    }

    // Imprimir los totales
    printf("Totales del sistema:\n");
    printf("  Memoria Reservada Total: %lu MB\n", totals.total_reserved_mb);
    printf("  Memoria Comprometida Total: %lu MB\n", totals.total_committed_mb);

    return EXIT_SUCCESS;
}

