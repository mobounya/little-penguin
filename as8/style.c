#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

char file_content_buffer[PAGE_SIZE];

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs)
{
	ssize_t res;

	res = simple_write_to_buffer(file_content_buffer, size, offs, user, size);
	if (res > 0)
		// 0x0 = '\0'
		file_content_buffer[res] = 0x0;
	return res;
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs){
	char *tmp;
	long t;
	size_t i;


	tmp = kmalloc(sizeof(char) * PAGE_SIZE * 2, GFP_KERNEL);
	t = strlen(file_content_buffer);
	if (t == 0)
		return 0;
	for (t = t - 1, i = 0; t >= 0; t--, i++)
		tmp[i] = file_content_buffer[t];
	tmp[i] = 0x0;
	return simple_read_from_buffer(user, size, offs, tmp, i);
}

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE, 
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&(*(&(myfd_device))));
	return retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

module_init(myfd_init);
module_exit(myfd_cleanup);
