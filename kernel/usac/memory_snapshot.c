#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/swap.h>

struct mem_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long active_pages;
    unsigned long inactive_pages;
    unsigned long swap_total;
    unsigned long swap_cached;
    unsigned long swap_used;
};

SYSCALL_DEFINE1(capture_memory_snapshot, struct mem_snapshot __user *, snapshot)
{
    struct mem_snapshot snap;
    struct sysinfo si;

    si_meminfo(&si);
    si_swapinfo(&si);

    snap.total_memory = si.totalram << (PAGE_SHIFT - 10); // Convert to KB
    snap.free_memory = si.freeram << (PAGE_SHIFT - 10);
    snap.active_pages = global_node_page_state(NR_ACTIVE_ANON) + 
                        global_node_page_state(NR_ACTIVE_FILE);
    snap.inactive_pages = global_node_page_state(NR_INACTIVE_ANON) + 
                          global_node_page_state(NR_INACTIVE_FILE);
    snap.swap_total = si.totalswap << (PAGE_SHIFT - 10);
    snap.swap_cached = total_swapcache_pages() << (PAGE_SHIFT - 10);
    snap.swap_used =  (si.totalswap - si.freeswap) << (PAGE_SHIFT - 10);
    if (copy_to_user(snapshot, &snap, sizeof(snap)))
        return -EFAULT;

    return 0;
}
