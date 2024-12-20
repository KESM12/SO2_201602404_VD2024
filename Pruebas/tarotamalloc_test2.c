#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#define SYS_TAMALLOC 551

void print_rss() {
    char path[256];
    char buffer[1024];
    sprintf(path, "/proc/%d/status", getpid());

    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error al abrir /proc/<PID>/status");
        return;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "VmRSS:", 6) == 0) {
            printf("%s", buffer); // Imprime el RSS actual
            break;
        }
    }
    fclose(file);
}

int main() {
    size_t size = 500 * 1024 * 1024; // 500 MB

    printf("Antes de asignar memoria:\n");
    print_rss();

    // Llamar a la syscall para asignar memoria
    void *ptr = (void *)syscall(SYS_TAMALLOC, size);
    if ((long)ptr < 0) {
        perror("Error en tamalloc");
        return 1;
    }

    printf("Después de asignar memoria (sin acceder):\n");
    print_rss();

    // Acceder a la memoria asignada
    char *buffer = (char *)ptr;
    for (size_t i = 0; i < size; i += (1024 * 1024)) { // Acceder a cada MB
        buffer[i] = 'A'; // Esto debería causar un page-fault por cada página
    }

    printf("Después de acceder a la memoria:\n");
    print_rss();
    
    return 0;
}
