#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#define TARO_ADD_MEMORY_LIMIT 450

int main() {
    pid_t pid = 1234; // PID de prueba
    long limit = 1024 * 1024; // 1 MB

    long result = syscall(TARO_ADD_MEMORY_LIMIT, pid, limit);
    if (result == 0) {
        printf("Límite de memoria agregado correctamente.\n");
    } else {
        printf("Error: %ld\n", result);
    }
    return 0;
}
