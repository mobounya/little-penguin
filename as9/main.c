#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/string.h>
#include <linux/debugfs.h>

#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mobounya");
MODULE_DESCRIPTION("Print all mount points");

int hello_init(void)
{
    struct path path;
    struct dentry *root_dentry;
    struct dentry *curdentry;

    kern_path("/", LOOKUP_FOLLOW, &path);
    root_dentry = path.dentry;
    list_for_each_entry(curdentry, &current->fs->root.mnt->mnt_root->d_subdirs, d_child) {
        printk("%s\n", curdentry->d_name.name);
    }
    return 0;
}

void hello_cleanup(void)
{
    printk("Cleaning up device module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
