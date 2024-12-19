#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/uaccess.h>
#include <linux/types.h>

SYSCALL_DEFINE1(taro_tamalloc, size_t, size) {
    unsigned long addr;
    size_t aligned_size;
    unsigned long populate = 0;  // Para manejar el mapeo inicial
    vm_flags_t vm_flags = 0;     // Bandera de las propiedades de mapeo

    // Verificar si el tamaño solicitado es válido
    if (size == 0) {
        printk(KERN_ERR "taro_tamalloc: Tamaño de memoria no puede ser 0.\n");
        return -EINVAL; // Error de argumento inválido
    }

    // Alinear el tamaño solicitado al tamaño de página
    aligned_size = PAGE_ALIGN(size);

    // Comprobar si la alineación falló
    if (aligned_size == 0) {
        printk(KERN_ERR "taro_tamalloc: Error en la alineación del tamaño.\n");
        return -ENOMEM; // Error de memoria insuficiente
    }

    // Configurar las banderas de protección y tipo de mapeo
    vm_flags = VM_READ | VM_WRITE | VM_MAYREAD | VM_MAYWRITE;

    // Mapear memoria sin reservar páginas físicas (lazy allocation)
    addr = do_mmap(NULL, 0, aligned_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                   vm_flags, 0, &populate, NULL);

    // Verificar si el mapeo fue exitoso
    if (IS_ERR_VALUE(addr)) {
        printk(KERN_ERR "taro_tamalloc: Error al mapear la memoria virtual.\n");
        return addr; // Retornar el código de error
    }

    // Mensaje de éxito
    printk(KERN_INFO "taro_tamalloc: Memoria asignada exitosamente en 0x%lx, tamaño: %zu bytes\n", addr, aligned_size);

    return addr; // Dirección de memoria virtual asignada
}

