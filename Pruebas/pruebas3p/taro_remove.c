#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>

#define TARO_REMOVE_MEMORY_LIMIT 560

int main() {
    pid_t process_pid;
    char continuar;

    printf("=== Prueba de syscall taro_remove_memory_limit ===\n");

    do {
        // Solicitar el PID del proceso
        printf("Ingrese el PID del proceso a eliminar: ");
        if (scanf("%d", &process_pid) != 1 || process_pid <= 0) {
            fprintf(stderr, "Error: El PID debe ser un número entero positivo.\n");
            return 1;
        }

        // Llamar a la syscall
        long result = syscall(TARO_REMOVE_MEMORY_LIMIT, process_pid);

        // Manejo de resultados
        if (result == 0) {
            printf("Límite de memoria eliminado correctamente para el PID %d.\n", process_pid);
        } else {
            perror("Error al eliminar el límite de memoria");
            switch (errno) {
                case ESRCH:
                    fprintf(stderr, "Error: Proceso con PID %d no encontrado.\n", process_pid);
                    break;
                case 102:
                    fprintf(stderr, "Error: El proceso no está en la lista de procesos limitados.\n");
                    break;
                case EINVAL:
                    fprintf(stderr, "Error: PID inválido.\n");
                    break;
                case EPERM:
                    fprintf(stderr, "Error: Permiso denegado. Se requieren privilegios de superusuario.\n");
                    break;
                default:
                    fprintf(stderr, "Error desconocido: %d.\n", errno);
            }
        }

        // Preguntar si desea realizar otra prueba
        printf("\n¿Desea probar otro proceso? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');

    return 0;
}

