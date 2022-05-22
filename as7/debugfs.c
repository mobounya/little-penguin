#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/string.h>
#include <linux/debugfs.h>

#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mobounya");
MODULE_DESCRIPTION("A misc module");

ssize_t debugfs_id_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
    size_t i;
    char *login = "mobounya";
    for (i = 0; i < count; i++)
        buf[i] = login[i];
    return i;
}

ssize_t debugfs_id_write(struct file *file, const char __user *buf,
               size_t len, loff_t *ppos)
{
    printk("Writing to penguin misc device...");
    if (strcmp(buf, "mobounya") == 0)
        return 9;
    else
        return -1; 
}

ssize_t debugfs_jiffies_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
    return jiffies;
}

struct file_operations id_fops = {
    .read = debugfs_id_read,
    .write = debugfs_id_write
};

struct file_operations jiffies_fops = {
    .read = debugfs_jiffies_read
};

int hello_init(void)
{
    struct dentry *fortytwo_dir = debugfs_create_dir("fortytwo", NULL);
    debugfs_create_file("id", S_IRWXU | S_IRWXG | S_IRWXO, fortytwo_dir, NULL, &id_fops);
    debugfs_create_file("jiffies", S_IRUSR | S_IRGRP | S_IROTH, fortytwo_dir, NULL, &jiffies_fops);
    return 0;
}

void hello_cleanup(void)
{
    printk("Cleaning up device module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
