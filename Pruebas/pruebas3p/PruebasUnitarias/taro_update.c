#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>

#define TARO_UPDATE_MEMORY_LIMIT 559

int main() {
    pid_t process_pid;
    size_t memory_limit;
    char continuar;

    printf("=== Prueba de syscall taro_update_memory_limit ===\n");

    do {
        // Solicitar el PID del proceso
        printf("Ingrese el PID del proceso: ");
        if (scanf("%d", &process_pid) != 1 || process_pid <= 0) {
            fprintf(stderr, "Error: PID debe ser un número entero positivo.\n");
            return 1;
        }

        // Solicitar el nuevo límite de memoria
        printf("Ingrese el nuevo límite de memoria en MB: ");
        if (scanf("%zu", &memory_limit) != 1 || memory_limit <= 0) {
            fprintf(stderr, "Error: El límite de memoria debe ser un número positivo.\n");
            return 1;
        }

        // Convertir el límite de memoria a bytes
        memory_limit *= 1024 * 1024;

        // Llamar a la syscall
        long result = syscall(TARO_UPDATE_MEMORY_LIMIT, process_pid, memory_limit);

        // Manejo de resultados
        if (result == 0) {
            printf("Límite de memoria actualizado correctamente para el PID %d.\n", process_pid);
        } else {
            perror("Error al actualizar el límite de memoria");
            switch (errno) {
                case ESRCH:
                    fprintf(stderr, "Error: Proceso con PID %d no encontrado.\n", process_pid);
                    break;
                case 100:
                    fprintf(stderr, "Error: El proceso ya excede el nuevo límite de memoria.\n");
                    break;
                case 102:
                    fprintf(stderr, "Error: El proceso no está en la lista de procesos limitados.\n");
                    break;
                case EINVAL:
                    fprintf(stderr, "Error: PID o límite de memoria inválidos.\n");
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