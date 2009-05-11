#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

/* local headers */
#include <main.h>
#include <inode.h>
#include <device.h>
#include <vfs.h>

/* global stuff */
struct list_head fmon_list;
struct file_monitor *fmon;

static int __init
fmon_init(void)
{
	int retval;

	retval = register_fmon_dev();
	if (retval < 0)
		goto out;

	fmon = kmalloc(sizeof(struct file_monitor), GFP_KERNEL);
	INIT_LIST_HEAD(&fmon->event_list);

	fmon->sb = find_init_superblock();

	retval = assign_inode_op(fmon, fmon->sb->s_root);
	if (retval > 0) {
		printk(KERN_INFO "assigned !!!\n");
	}

out:
	return 0;
}

static void __exit
fmon_exit(void)
{
	unregister_fmon_dev();
	restore_inode_op(fmon);
	kfree(fmon);
}

module_init(fmon_init);
module_exit(fmon_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Uladzislau Rezki");
