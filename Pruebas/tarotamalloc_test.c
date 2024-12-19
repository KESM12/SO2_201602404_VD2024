
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>

#define SYS_TAMALLOC 551
int main() {
    printf("PID: %d asignado a taro_Tamalloc.\n", getpid());

    printf("Implementación de Tamalloc para asignación de memoria. Presione ENTER para continuar...\n");
    getchar();

    size_t total_size = 100 * 1024 * 1024; // 100 MB

    // Use the tamalloc syscall
    char *buffer = (char *)syscall(SYS_TAMALLOC, total_size);
    if ((long)buffer < 0) {
        perror("Error en Tamalloc");
        return 1;
    }
    printf("Alojamiento de memoria (%zu bytes) usando Tamalloc en la dirección: %p\n", total_size, buffer);
    printf("Presione ENTER para comenzar a leer la memoria byte a byte...\n");
    getchar();

    srand(time(NULL));

    // Read memory byte by byte and verify it is zero
    for (size_t i = 0; i < total_size; i++) {
        char t = buffer[i]; // triggers lazy allocation (with zeroing :D )
        if (t != 0) {
            printf("ERROR FATAL: La memoria en el byte %zu no se inicializó a 0.\n", i);
            return 10;
        }

        //Write a random A-Z char to trigger CoW
        char random_letter = 'A' + (rand() % 26);
        buffer[i] = random_letter;

        if (i % (1024 * 1024) == 0 && i > 0) { // Every 1 MB
            printf("%zu MB Comprobados\n", i / (1024 * 1024));
            sleep(1);
        }
    }

    printf("Se verificó que toda la memoria se inicializó a cero. Presione ENTER para salir.\n");
    getchar();
    return 0;
}
