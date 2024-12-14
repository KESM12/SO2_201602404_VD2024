#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <time.h>  
#include <sys/types.h>

struct timespec64 {
    long tv_sec;   
    long tv_nsec;  
};

struct syscall_usage {
    unsigned long count;                    
    struct timespec64 time_last_used;       
};

#define MAX_SYS_CALLS 4
#define __NR_track_syscall_usage 549

int main() {
    struct syscall_usage statistics[MAX_SYS_CALLS];

    if (syscall(__NR_track_syscall_usage, statistics) < 0) {
        perror("Error en racso_track_syscall_usage");
        return EXIT_FAILURE;
    }

    // Imprimir resultados
    for (int i = 0; i < MAX_SYS_CALLS; i++) {
        printf("Syscall ID %d: Count = %lu\n", i, statistics[i].count);
        printf("Última vez usada: %ld.%09ld\n", 
               statistics[i].time_last_used.tv_sec, 
               statistics[i].time_last_used.tv_nsec);
    }

    return EXIT_SUCCESS;
}


