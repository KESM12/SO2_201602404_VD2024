#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#define TARO_UPDATE_MEMORY_LIMIT 559

int main() {
    pid_t process_pid = 1234; // PID del proceso al que deseas actualizar el límite
    size_t memory_limit = 1024 * 1024; // Nuevo límite de memoria (1 MB)

    long result = syscall(TARO_UPDATE_MEMORY_LIMIT, process_pid, memory_limit);
    if (result == 0) {
        printf("Límite de memoria actualizado correctamente.\n");
    } else {
        perror("Error al actualizar el límite de memoria");
    }

    return 0;
}
