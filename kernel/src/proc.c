#include <linux/proc_fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>

/* locals */
#include <proc.h>
#include <inode.h>

static struct proc_dir_entry *root_folder;

static int
state_read(char *page, char **start, off_t off, int count, int *eof,
		   void *data)
{
	int len;

	len = sprintf(page, "%s\n", is_active ? "started":"stopped");

	return len;
}

static int
state_write(struct file *filp, const char __user *buff, unsigned long count,
			void *data)
{
	char c[64] = {'\0'};

	if (copy_from_user(c, buff, sizeof(c)))
		return -EFAULT;

	/* this is for test purposes only */
	if (!memcmp(c, "start", 5)) {
		is_active = 1;
		return count;
	} else if (!memcmp(c, "stop", 4)) {
		is_active = 0;
		return count;
	}

	return -EINVAL;
}

int register_proc_entries(void)
{
	struct proc_dir_entry *entry;

	root_folder = proc_mkdir("fmon", NULL);
	if (root_folder == NULL)
		goto out;

	entry = create_proc_entry("state", 0644, root_folder);
	if (entry) {
		entry->read_proc = state_read;
		entry->write_proc = state_write;
		entry->owner = THIS_MODULE;
	}

	return 0;
out:
	return -1;
}

int unregister_proc_entries(void)
{
	remove_proc_entry("state", root_folder);
	remove_proc_entry("fmon", NULL);
}
