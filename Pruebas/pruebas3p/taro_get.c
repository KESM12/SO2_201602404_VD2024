#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define TARO_GET_MEMORY_LIMITS 558

struct memory_limitation {
    pid_t pid;
    long limit;
};

int main() {
    size_t max_entries;
    printf("Enter max entries: ");
    if (scanf("%zu", &max_entries) != 1 || max_entries <= 0) {
        fprintf(stderr, "Invalid max entries.\n");
        return -1;
    }

    struct memory_limitation buffer[max_entries];
    int processes_returned;

    // Llamar a la syscall
    long result = syscall(TARO_GET_MEMORY_LIMITS, buffer, max_entries, &processes_returned);
    if (result < 0) {
        perror("syscall failed");
        switch (errno) {
            case EINVAL:
                fprintf(stderr, "Error: Invalid parameters (EINVAL).\n");
                break;
            case ENOMEM:
                fprintf(stderr, "Error: Insufficient memory (ENOMEM).\n");
                break;
            case EFAULT:
                fprintf(stderr, "Error: Failed to copy data (EFAULT).\n");
                break;
            default:
                fprintf(stderr, "Error: Unknown error (%d).\n", errno);
        }
        return -1;
    }

    printf("Processes returned: %d\n", processes_returned);
    if (processes_returned > max_entries) {
        fprintf(stderr, "Error: Returned more processes than the buffer can hold.\n");
        return -1;
    }

    for (int i = 0; i < processes_returned; i++) {
        printf("PID: %d, Limit: %ld\n", buffer[i].pid, buffer[i].limit);
    }

    return 0;
}
