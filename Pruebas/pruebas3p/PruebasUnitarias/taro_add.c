#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define taro_add 557 // Número de syscall

int main() {
    pid_t user_pid;
    size_t memory_limit_mb, memory_limit_bytes;
    char continuar;

    printf("=== Prueba de syscall taro_add_memory_limit ===\n");

    do {
        printf("Ingrese el PID del proceso (debe ser positivo): ");
        if (scanf("%d", &user_pid) != 1 || user_pid <= 0) {
            fprintf(stderr, "Error: El PID debe ser un número entero positivo.\n");
            return 1;
        }

        printf("Ingrese el límite de memoria en MB: ");
        if (scanf("%zu", &memory_limit_mb) != 1 || memory_limit_mb <= 0) {
            fprintf(stderr, "Error: El límite de memoria debe ser un número positivo.\n");
            return 1;
        }

        memory_limit_bytes = memory_limit_mb * 1024 * 1024;

        printf("\nProbando syscall taro_add_memory_limit\n");
        printf("PID ingresado: %d\n", user_pid);
        printf("Límite de memoria ingresado: %zu MB (%zu bytes)\n", memory_limit_mb, memory_limit_bytes);

        long result = syscall(taro_add, user_pid, memory_limit_bytes);

        if (result == 0) {
            printf("Syscall completada con éxito.\n");
        } else {
            perror("syscall failed");
            switch (errno) {
                case ESRCH:
                    fprintf(stderr, "Error: Proceso con PID %d no encontrado.\n", user_pid);
                    break;
                case EPERM:
                    fprintf(stderr, "Error: No tienes permisos suficientes para esta operación.\n");
                    break;
                case ENOMEM:
                    fprintf(stderr, "Error: Sin memoria suficiente para registrar el límite.\n");
                    break;
                case EINVAL:
                    fprintf(stderr, "Error: Parámetros inválidos (PID o límite).\n");
                    break;
                case 100:
                    fprintf(stderr, "Error: El proceso ya excede el límite de memoria especificado.\n");
                    break;
                case 101:
                    fprintf(stderr, "Error: El proceso ya tiene un límite registrado.\n");
                    break;
                default:
                    fprintf(stderr, "Error desconocido: %d\n", errno);
            }
        }

        printf("\n¿Desea probar otro proceso? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');

    return 0;
}
