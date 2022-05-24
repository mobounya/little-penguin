#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/string.h>
#include <linux/debugfs.h>

#include <linux/jiffies.h>

#include <linux/slab.h>
#include <linux/kernel.h>

#include <linux/spinlock.h>
#include <linux/syscalls.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("mobounya");
MODULE_DESCRIPTION("A debugfs module");


char foo_data_buffer[PAGE_SIZE] = { '\0' };
size_t foo_buffer_capacity = PAGE_SIZE;
size_t foo_buffer_size = 0;

struct dentry *fortytwo_dir;

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

ssize_t debugfs_foo_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
    size_t i;

    spinlock_t foo_lock;
    spin_lock_init(&foo_lock);

    spin_lock(&foo_lock);
    for (i = 0; i < count && i < foo_buffer_size; i++)
        buf[i] = foo_data_buffer[i];
    spin_unlock(&foo_lock);
    return i;
}

ssize_t debugfs_foo_write(struct file *file, const char __user *buf,
               size_t len, loff_t *ppos)
{
    size_t i;

    spinlock_t foo_lock;
    spin_lock_init(&foo_lock);

    spin_lock(&foo_lock);
    if (len > PAGE_SIZE)
        len = PAGE_SIZE;
    foo_buffer_size = len;
    for (i = 0; i < len; i++)
        foo_data_buffer[i] = buf[i];
    spin_unlock(&foo_lock);
    return i;
}

struct file_operations id_fops = {
    .read = debugfs_id_read,
    .write = debugfs_id_write
};

struct file_operations jiffies_fops = {
    .read = debugfs_jiffies_read
};

struct file_operations foo_fops = {
    .write = debugfs_foo_write,
    .read = debugfs_foo_read
};

int hello_init(void)
{
    struct dentry *id_file;
    struct dentry *jiffies_file;
    struct dentry *foo_file;

    fortytwo_dir = debugfs_create_dir("fortytwo", NULL);
    if (fortytwo_dir == (struct dentry *)ERR_PTR)
        return 1;
    id_file = debugfs_create_file("id", S_IRWXU | S_IRWXG | S_IRWXO, fortytwo_dir, NULL, &id_fops);
    if (id_file == (struct dentry *)ERR_PTR)
        return 1;
    jiffies_file = debugfs_create_file("jiffies", S_IRUSR | S_IRGRP | S_IROTH, fortytwo_dir, NULL, &jiffies_fops);
    if (jiffies_file == (struct dentry *)ERR_PTR)
        return 1; 
    foo_file = debugfs_create_file("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, fortytwo_dir, NULL, &foo_fops);
    if (foo_file == (struct dentry *)ERR_PTR)
        return 1; 
    return 0;
}


void hello_cleanup(void)
{
    debugfs_remove_recursive(fortytwo_dir);
}

module_init(hello_init);
module_exit(hello_cleanup);