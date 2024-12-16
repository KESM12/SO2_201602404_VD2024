#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <syscall.h>

// Estructura que coincide con la definición del kernel
struct syscall_usage {
    unsigned long count;
    struct timespec time_last_used;
};

#define MAX_SYS_CALLS 4

// Nombres de las llamadas al sistema para impresión
const char *syscall_names[] = {
    "open",
    "write", 
    "read",
    "fork"
};

int main() {
    struct syscall_usage statistics[MAX_SYS_CALLS];

    // Llamar al syscall personalizado para obtener estadísticas
    long result = syscall(549, statistics);
    
    if (result != 0) {
        perror("Error al obtener estadísticas de llamadas al sistema");
        return 1;
    }

    // Imprimir resultados
    printf("Estadísticas de llamadas al sistema:\n");
    for (int i = 0; i < MAX_SYS_CALLS; i++) {
        printf("%s:\n", syscall_names[i]);
        printf("  Número de llamadas: %lu\n", statistics[i].count);
        
        // Convertir timespec a tiempo legible
        char buffer[64];
        struct tm *tmp = localtime(&statistics[i].time_last_used.tv_sec);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tmp);
        
        printf("  Última llamada: %s.%09ld\n", 
               buffer, 
               statistics[i].time_last_used.tv_nsec);
    }

    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/syscall.h>
// #include <string.h>
// #include <time.h>  
// #include <sys/types.h>

// struct timespec64 {
//     long tv_sec;   
//     long tv_nsec;  
// };

// struct syscall_usage {
//     unsigned long count;                    
//     struct timespec64 time_last_used;       
// };

// #define MAX_SYS_CALLS 4
// #define __NR_track_syscall_usage 549

// int main() {
//     struct syscall_usage statistics[MAX_SYS_CALLS];

//     if (syscall(__NR_track_syscall_usage, statistics) < 0) {
//         perror("Error en racso_track_syscall_usage");
//         return EXIT_FAILURE;
//     }

//     // Imprimir resultados
//     for (int i = 0; i < MAX_SYS_CALLS; i++) {
//         printf("Syscall ID %d: Count = %lu\n", i, statistics[i].count);
//         printf("Última vez usada: %ld.%09ld\n", 
//                statistics[i].time_last_used.tv_sec, 
//                statistics[i].time_last_used.tv_nsec);
//     }

//     return EXIT_SUCCESS;
// }


