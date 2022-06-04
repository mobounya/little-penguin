#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/list.h>
#include <linux/fs_struct.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/nsproxy.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/ns_common.h>
#include <linux/fs_pin.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define NUMBER_OF_SPACES 20

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mobounya");
MODULE_DESCRIPTION("Print all mount points");

struct mnt_namespace
{
	struct ns_common ns;
	struct mount *root;
	/*
	 * Traversal and modification of .list is protected by either
	 * - taking namespace_sem for write, OR
	 * - taking namespace_sem for read AND taking .ns_lock.
	 */
	struct list_head list;
	spinlock_t ns_lock;
	struct user_namespace *user_ns;
	struct ucounts *ucounts;
	u64 seq; /* Sequence number to prevent loops */
	wait_queue_head_t poll;
	u64 event;
	unsigned int mounts; /* # of mounts in the namespace */
	unsigned int pending_mounts;
} __randomize_layout;

struct mount
{
	struct hlist_node mnt_hash;
	struct mount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct vfsmount mnt;
	union
	{
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	 /* list of children, anchored here */
	struct list_head mnt_child;		 /* and going through their mnt_child */
	struct list_head mnt_instance; /* mount instance on sb->s_mounts */
	const char *mnt_devname;			 /* Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list;
	struct list_head mnt_expire;		 /* link in fs-specific expiry list */
	struct list_head mnt_share;			 /* circular list of shared mounts */
	struct list_head mnt_slave_list; /* list of slave mounts */
	struct list_head mnt_slave;			 /* slave list entry */
	struct mount *mnt_master;				 /* slave is on master->mnt_slave_list */
	struct mnt_namespace *mnt_ns;		 /* containing namespace */
	struct mountpoint *mnt_mp;			 /* where is it mounted */
	union
	{
		struct hlist_node mnt_mp_list; /* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;					 /* mount identifier */
	int mnt_group_id;		 /* peer group identifier */
	int mnt_expiry_mark; /* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

char proc_file_buffer[PAGE_SIZE];
size_t proc_file_buffer_capacity = PAGE_SIZE;
size_t proc_file_buffer_size = 0;

ssize_t my_proc_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	struct mount *current_mnt;
	char *path;
	char *buffer;
	size_t offset = 0;
	int name_len = 0;
	unsigned int i;

	if (proc_file_buffer_size > 0)
	{
		if (*offs >= proc_file_buffer_size)
			return 0;
		return simple_read_from_buffer(user, size, offs, proc_file_buffer, PAGE_SIZE);
	}

	list_for_each_entry(current_mnt, &current->nsproxy->mnt_ns->list, mnt_list)
	{
		buffer = kmalloc(sizeof(char) * 256, GFP_KERNEL);
		path = dentry_path_raw(current_mnt->mnt_mountpoint, buffer, 256);

		name_len = strlen(current_mnt->mnt_devname);
		memcpy(proc_file_buffer + offset, current_mnt->mnt_devname, name_len);
		offset += name_len;

		// Print number of spaces (padding) depending on how long the name is.
		for (i = 0; i < NUMBER_OF_SPACES - name_len; i++)
		{
			memcpy(proc_file_buffer + offset, " ", 1);
			offset += 1;
		}

		memcpy(proc_file_buffer + offset, path, strlen(path));
		offset += strlen(path);

		memcpy(proc_file_buffer + offset, "                    ", 20);
		offset += 20;

		buffer = kmalloc(sizeof(char) * 256, GFP_KERNEL);
		path = dentry_path_raw(current_mnt->mnt_parent->mnt_mountpoint, buffer, 256);

		memcpy(proc_file_buffer + offset, path, strlen(path));
		offset += strlen(path);

		memcpy(proc_file_buffer + offset, "\n", 1);
		offset += 1;
	}
	proc_file_buffer_size += offset;
	return simple_read_from_buffer(user, size, offs, proc_file_buffer, offset);
}

struct proc_ops proc_fops = {
		.proc_read = my_proc_read,
};

int hello_init(void)
{
	proc_create("mymounts", S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP, NULL, &proc_fops);

	return 0;
}

void hello_cleanup(void)
{
	remove_proc_entry("mymounts", NULL);
}

module_init(hello_init);
module_exit(hello_cleanup);
