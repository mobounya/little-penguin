#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

int hello_init(void)
{
    printk("Hello world!\n");
    return 0;
}

void hello_cleanup(void)
{
    printk("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
