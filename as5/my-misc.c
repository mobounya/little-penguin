// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/miscdevice.h>

#include <linux/fs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mobounya");
MODULE_DESCRIPTION("A misc device driver");

ssize_t misc_dev_read(struct file *filp, char __user *buf,
			size_t count, loff_t *f_pos)
{
	char *login = "mobounya";
	size_t size = 8;

	return simple_read_from_buffer(buf, count, f_pos, login, size);
}

ssize_t misc_dev_write(struct file *file, const char __user *buf,
			size_t len, loff_t *ppos)
{
	char buffer[9] = { '\0' };
	ssize_t ret;

	ret = simple_write_to_buffer(buffer, 8, ppos, buf, len);
	if (strcmp("mobounya", buffer) == 0)
		return ret;
	else
		return -1;
}

const struct file_operations fops = {
		.read = misc_dev_read,
		.write = misc_dev_write,
};

struct miscdevice misc_dev = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = "fortytwo",
		.fops = &fops,
};

int hello_init(void)
{
	int error;

	printk(KERN_INFO "Init misc device penguin\n");
	error = misc_register(&misc_dev);
	if (error) {
		pr_err("misc_register failed");
		return error;
	}
	return 0;
}

void hello_cleanup(void)
{
	printk(KERN_INFO "Cleaning up misc device module.\n");
	misc_deregister(&misc_dev);
}

module_init(hello_init);
module_exit(hello_cleanup);
