#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

struct mem_snapshot {
    unsigned long total_memory;   // in KB
    unsigned long free_memory;    // in KB
    unsigned long active_pages;
    unsigned long inactive_pages;
};

int main() {
    struct mem_snapshot snapshot;

    // Call the syscall with ID 548
    if (syscall(548, &snapshot) == 0) {
        // Conversion factor from KB to GB
        const double KB_TO_GB = 1024.0 * 1024.0;

        // Print memory stats in GB
        printf("Total Memory: %.2f GB\n", snapshot.total_memory / KB_TO_GB);
        printf("Free Memory: %.2f GB\n", snapshot.free_memory / KB_TO_GB);

        // Print page stats as-is
        printf("Active Pages: %lu\n", snapshot.active_pages);
        printf("Inactive Pages: %lu\n", snapshot.inactive_pages);
    } else {
        perror("Syscall failed");
    }

    return 0;
}
