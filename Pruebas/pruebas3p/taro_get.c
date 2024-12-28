#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

#define TARO_GET_MEMORY_LIMITS 451

struct memory_limitation {
    pid_t pid;
    long limit;
};

int main() {
    size_t max_entries = 10;
    struct memory_limitation buffer[max_entries];
    int processes_returned;

    // Llamar a la syscall
    long result = syscall(TARO_GET_MEMORY_LIMITS, buffer, max_entries, &processes_returned);
    if (result < 0) {
        perror("syscall failed");
        return -1;
    }

    printf("Processes returned: %d\n", processes_returned);
    for (int i = 0; i < processes_returned; i++) {
        printf("PID: %d, Limit: %ld\n", buffer[i].pid, buffer[i].limit);
    }

    return 0;
}
