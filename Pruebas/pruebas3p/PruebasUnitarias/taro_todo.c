#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define TARO_ADD_MEMORY_LIMIT 557
#define TARO_GET_MEMORY_LIMITS 558
#define TARO_UPDATE_MEMORY_LIMIT 559
#define TARO_REMOVE_MEMORY_LIMIT 560

struct memory_limitation {
    pid_t pid;
    size_t limit;
};

void test_add_memory_limit() {
    pid_t user_pid;
    size_t memory_limit_mb, memory_limit_bytes;
    char continuar;

    printf("=== Prueba de syscall taro_add_memory_limit ===\n");

    do {
        printf("Ingrese el PID del proceso (debe ser positivo): ");
        if (scanf("%d", &user_pid) != 1 || user_pid <= 0) {
            fprintf(stderr, "Error: El PID debe ser un número entero positivo.\n");
            return;
        }

        printf("Ingrese el límite de memoria en MB: ");
        if (scanf("%zu", &memory_limit_mb) != 1 || memory_limit_mb <= 0) {
            fprintf(stderr, "Error: El límite de memoria debe ser un número positivo.\n");
            return;
        }

        memory_limit_bytes = memory_limit_mb * 1024 * 1024;

        printf("\nProbando syscall taro_add_memory_limit\n");
        printf("PID ingresado: %d\n", user_pid);
        printf("Límite de memoria ingresado: %zu MB (%zu bytes)\n", memory_limit_mb, memory_limit_bytes);

        long result = syscall(TARO_ADD_MEMORY_LIMIT, user_pid, memory_limit_bytes);

        if (result == 0) {
            printf("Syscall completada con éxito.\n");
        } else {
            perror("syscall failed");
        }

        printf("\n¿Desea probar otro proceso? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');
}

void test_get_memory_limits() {
    size_t max_entries;
    printf("=== Prueba de syscall taro_get_memory_limits ===\n");
    printf("Enter max entries: ");
    if (scanf("%zu", &max_entries) != 1 || max_entries <= 0) {
        fprintf(stderr, "Invalid max entries.\n");
        return;
    }

    struct memory_limitation buffer[max_entries];
    int processes_returned;

    long result = syscall(TARO_GET_MEMORY_LIMITS, buffer, max_entries, &processes_returned);
    if (result < 0) {
        perror("syscall failed");
        return;
    }

    printf("Processes returned: %d\n", processes_returned);
    for (int i = 0; i < processes_returned; i++) {
        printf("PID: %d, Limit: %ld\n", buffer[i].pid, buffer[i].limit);
    }
}

void test_update_memory_limit() {
    pid_t process_pid;
    size_t memory_limit;
    char continuar;

    printf("=== Prueba de syscall taro_update_memory_limit ===\n");

    do {
        printf("Ingrese el PID del proceso: ");
        if (scanf("%d", &process_pid) != 1 || process_pid <= 0) {
            fprintf(stderr, "Error: PID debe ser un número entero positivo.\n");
            return;
        }

        printf("Ingrese el nuevo límite de memoria en MB: ");
        if (scanf("%zu", &memory_limit) != 1 || memory_limit <= 0) {
            fprintf(stderr, "Error: El límite de memoria debe ser un número positivo.\n");
            return;
        }

        memory_limit *= 1024 * 1024;

        long result = syscall(TARO_UPDATE_MEMORY_LIMIT, process_pid, memory_limit);

        if (result == 0) {
            printf("Límite de memoria actualizado correctamente para el PID %d.\n", process_pid);
        } else {
            perror("Error al actualizar el límite de memoria");
        }

        printf("\n¿Desea probar otro proceso? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');
}

void test_remove_memory_limit() {
    pid_t process_pid;
    char continuar;

    printf("=== Prueba de syscall taro_remove_memory_limit ===\n");

    do {
        printf("Ingrese el PID del proceso a eliminar: ");
        if (scanf("%d", &process_pid) != 1 || process_pid <= 0) {
            fprintf(stderr, "Error: El PID debe ser un número entero positivo.\n");
            return;
        }

        long result = syscall(TARO_REMOVE_MEMORY_LIMIT, process_pid);

        if (result == 0) {
            printf("Límite de memoria eliminado correctamente para el PID %d.\n", process_pid);
        } else {
            perror("Error al eliminar el límite de memoria");
        }

        printf("\n¿Desea probar otro proceso? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');
}

int main() {
    int option;

    do {
        printf("\n=== Menú de Pruebas ===\n");
        printf("1. Agregar límite de memoria\n");
        printf("2. Obtener límites de memoria\n");
        printf("3. Actualizar límite de memoria\n");
        printf("4. Eliminar límite de memoria\n");
        printf("5. Salir\n");
        printf("Seleccione una opción: ");

        if (scanf("%d", &option) != 1) {
            fprintf(stderr, "Error: Entrada inválida.\n");
            return 1;
        }

        switch (option) {
            case 1:
                test_add_memory_limit();
                break;
            case 2:
                test_get_memory_limits();
                break;
            case 3:
                test_update_memory_limit();
                break;
            case 4:
                test_remove_memory_limit();
                break;
            case 5:
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opción inválida. Intente de nuevo.\n");
        }

    } while (option != 5);

    return 0;
}