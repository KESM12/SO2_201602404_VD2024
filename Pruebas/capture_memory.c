#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
};

int main() {
    struct mem_snapshot snapshot;
    if (syscall(548, &snapshot) == 0) {
        printf("Total Memory: %lu KB\n", snapshot.total_memory);
        printf("Free Memory: %lu KB\n", snapshot.free_memory);
        printf("Active Pages: %lu\n", snapshot.active_pages);
        printf("Inactive Pages: %lu\n", snapshot.inactive_pages);
    } else {
        perror("Syscall failed");
    }
    return 0;
}
