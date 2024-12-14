#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/statfs.h>
#include <linux/uaccess.h>
#include <linux/namei.h> // Para kern_path

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Secaida");
MODULE_DESCRIPTION("Estadísticas de almacenamiento en GB para una partición específica");
MODULE_VERSION("1.0");

static char *partition = "/"; // Partición por defecto
module_param(partition, charp, 0444);
MODULE_PARM_DESC(partition, "Ruta de la partición (por ejemplo, / o /mnt)");

static int write_storage_stats(struct seq_file *file, void *v)
{
    struct kstatfs stats;
    struct path path;

    // Resolver la ruta de la partición
    if (kern_path(partition, LOOKUP_FOLLOW, &path))
    {
        seq_printf(file, "{\n\"error\": \"No se pudo acceder a la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Obtener estadísticas de la partición
    if (vfs_statfs(&path, &stats))
    {
        seq_printf(file, "{\n\"error\": \"No se pudieron obtener estadísticas de la partición: %s\"\n}\n", partition);
        return 0;
    }

    // Calcular espacio total y libre en gigabytes
    unsigned long long total_space_gb = (stats.f_blocks * stats.f_bsize) >> 30; // Dividir entre 1024^3
    unsigned long long free_space_gb = (stats.f_bfree * stats.f_bsize) >> 30;  // Dividir entre 1024^3
    unsigned long long available_space_gb = (stats.f_bavail * stats.f_bsize) >> 30; // Dividir entre 1024^3

    // Escribir la salida en formato JSON
    seq_printf(file, "{\n");
    seq_printf(file, "\"partition\": \"%s\",\n", partition);
    seq_printf(file, "\"total_space_gb\": %llu,\n", total_space_gb);
    seq_printf(file, "\"free_space_gb\": %llu,\n", free_space_gb);
    seq_printf(file, "\"available_space_gb\": %llu\n", available_space_gb);
    seq_printf(file, "}\n");

    return 0;
}

static int open_storage_stats(struct inode *inode, struct file *file)
{
    return single_open(file, write_storage_stats, NULL);
}

static const struct proc_ops storage_stats_ops = {
    .proc_open = open_storage_stats,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init storage_stats_init(void)
{
    // Crear el archivo en /proc
    struct proc_dir_entry *entry = proc_create("disk_so2_201602404", 0444, NULL, &storage_stats_ops);
    if (!entry)
    {
        pr_err("Error al crear el archivo /proc/disk_so2_201602404\n");
        return -ENOMEM;
    }

    pr_info("Módulo de estadísticas de almacenamiento cargado. Partición: %s\n", partition);
    return 0;
}

static void __exit storage_stats_exit(void)
{
    // Remover el archivo de /proc
    remove_proc_entry("disk_so2_201602404", NULL);
    pr_info("Módulo de estadísticas de almacenamiento descargado\n");
}

module_init(storage_stats_init);
module_exit(storage_stats_exit);
