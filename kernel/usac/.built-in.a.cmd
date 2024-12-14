savedcmd_kernel/usac/built-in.a := rm -f kernel/usac/built-in.a;  printf "kernel/usac/%s " memory_snapshot.o track_syscalls.o io_throttle.o | xargs ar cDPrST kernel/usac/built-in.a
