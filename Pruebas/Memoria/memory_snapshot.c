#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define INFO_FILE "taro_memory"

static struct proc_dir_entry *taro_info_file;

static int taro_infor_show(struct seq_file *theFile, void *v)
{
    seq_printf(theFile, "Llamada a la syscall 548\n");
    //aqui va todo lo de imprimir lo de la ram
    return 0;
}

static into taro_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, taro_info_show, NULL);
}

static const struct proc_ops taro_memory_ops = {
    .proc_open = taro_info_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init taro_memory_init(void)
{
    printk(KERN_INFO "SYSCALL1: Inicializando modulo\n");
    taro_info_file = proc_create(INFO_FILE, 0, NULL, &taro_memory_ops);
    if (!taro_info_file)
    {
        printk(KERN_ERR "SYSCALL2: Error al crear el archivo en /proc\n");
        return -ENOMEM;
    }
    return 0;
}

static void __exit taro_memory_exit(void)
{
    printk(KERN_INFO "SYSCALL2: deteniendo el modulo\n");
    proc_remove(taro_memory);
}

module_init(taro_memory_init);
module_exit(taro_memory_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Secaida");
MODULE_DESCRIPTION("Modulo para la syscall2");
MODULE_VERSION("0.1");
