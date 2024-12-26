
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define SYS_TAMALLOC 551
int main()
{
    printf("PID: %d asignado a taro_Tamalloc.\n", getpid());

    printf("Implementación de Tamalloc para asignación de memoria. Presione ENTER para continuar...\n");
    getchar();

    size_t total_size = 21 * 1024 * 1024; // 20 MB

    // Use the tamalloc syscall
    char *buffer = (char *)syscall(SYS_TAMALLOC, total_size);
    if ((long)buffer < 0)
    {
        perror("Error en Tamalloc");
        return 1;
    }
    printf("Alojamiento de memoria (%zu bytes) usando Tamalloc en la dirección: %p\n", total_size, buffer);
    printf("Presione ENTER para comenzar a escribir 'kevin' y 'taro' aleatoriamente en la memoria...\n");
    getchar();

    srand(time(NULL));

    size_t i = 0;
    while (i < total_size)
    {
        // Randomly decide whether to write "kevin" or "taro"
        const char *str_to_write = (rand() % 2 == 0) ? "kevin" : "taro";
        size_t len = strlen(str_to_write);

        // Ensure we do not exceed the allocated buffer
        if (i + len > total_size)
        {
            break;
        }

        // Write the string to the buffer
        memcpy(buffer + i, str_to_write, len);
        i += len;

        if (i % (1024 * 1024) == 0 && i > 0)
        { // Every 1 MB
            printf("%zu MB Comprobados\n", i / (1024 * 1024));
            sleep(1);
        }
    }

    printf("Se verificó que toda la memoria se inicializó a cero. Presione ENTER para salir.\n");
    getchar();
    return 0;
}
