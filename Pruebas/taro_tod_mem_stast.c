#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Define la estructura para almacenar los resultados
struct total_mem_stats {
    unsigned long total_reserved_kb; // Memoria total reservada en KB
    unsigned long total_committed_kb; // Memoria total utilizada en KB
};

int main() {
    struct total_mem_stats stats;

    // Invocar el syscall
    int ret = syscall(553, &stats); 

    if (ret == 0) {
        // Mostrar los resultados
        printf("Memoria total reservada: %lu KB\n", stats.total_reserved_kb);
        printf("Memoria total utilizada: %lu KB\n", stats.total_committed_kb);
    } else {
        // Manejo de errores
        fprintf(stderr, "Error al invocar el syscall: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}



// #include <stdio.h>
// #include <unistd.h>
// #include <sys/syscall.h>
// #include <errno.h>

// struct system_mem_stats {
//     unsigned long total_reserved_mb;  // Memoria total reservada en MB
//     unsigned long total_committed_mb; // Memoria total utilizada en MB
// };

// int main() {
//     struct system_mem_stats stats;

//     if (syscall(553, &stats) == 0) {
//         printf("Estadísticas de memoria del sistema:\n");
//         printf("  Memoria total reservada: %lu MB\n", stats.total_reserved_mb);
//         printf("  Memoria total utilizada: %lu MB\n", stats.total_committed_mb);
//     } else {
//         perror("Error al obtener estadísticas de memoria del sistema");
//         return errno;
//     }

//     return 0;
// }
