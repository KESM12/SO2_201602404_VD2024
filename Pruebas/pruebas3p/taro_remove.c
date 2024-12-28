#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#define TARO_REMOVE_MEMORY_LIMIT 560

int main() {
    pid_t process_pid = 1234; // PID del proceso que deseas eliminar de la lista

    long result = syscall(TARO_REMOVE_MEMORY_LIMIT, process_pid);
    if (result == 0) {
        printf("Límite de memoria eliminado correctamente.\n");
    } else {
        perror("Error al eliminar el límite de memoria");
    }

    return 0;
}
