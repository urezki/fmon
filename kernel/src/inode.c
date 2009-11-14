#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>

/* local headers */
#include <dentry.h>
#include <event.h>
#include <main.h>

/* off */
int is_active = 0;

static int
fmon_create(struct inode *inode, struct dentry *dentry, int mode,
			struct nameidata *namei)
{
	struct event *event = NULL;
	int retval = -1;
	char *path;
	
	if (fmon->linux_create) {
		retval = fmon->linux_create(inode, dentry, mode, namei);
		if (!retval && is_active) {
			path = dentry_full_path(dentry);
			if (path) {
				event = create_event(E_CRE);

				strncpy(event->path_1, path, PATH_MAX);
				event->uid = inode->i_uid;
				event->gid = inode->i_gid;
				event->mode = mode;

				list_add(&event->list, &fmon->event_list);
				fmon->event_list_len++;
				printk(KERN_ERR "create [%d] --> %s mode: %d\n", current->pid, path, mode);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
fmon_link(struct dentry *d_1, struct inode *i, struct dentry *d_2)
{
	struct event *e;
	int retval = -1;
	char *path_d1;
	char *path_d2;
	
	if (fmon->linux_link) {
		retval = fmon->linux_link(d_1, i, d_2);
		if (!retval && is_active) {
			path_d1 = dentry_full_path(d_1);
			path_d2 = dentry_full_path(d_2);

			if (path_d1 && path_d2) {
				e = create_event(E_LIN);
				strncpy(e->path_1, path_d1, PATH_MAX);
				strncpy(e->path_2, path_d2, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "link [%d] --> %s to %s\n", current->pid, path_d1, path_d2);
				kfree(path_d1);
				kfree(path_d2);
			}
		}
	}
	
	return retval;
}

static int
fmon_unlink(struct inode *inode, struct dentry *dentry)
{
	struct event *e;
	int retval = -1;
	char *path;

	if (fmon->linux_unlink) {
		/* before real unlink, get the path */
		path = dentry_full_path(dentry);
		retval = fmon->linux_unlink(inode, dentry);
		if (!retval && is_active) {
			if (path) {
				e = create_event(E_UNL);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
				fmon->event_list_len++;
				printk(KERN_ERR "unlink [%d] --> %s\n", current->pid, path);
			}
		}
		
		if (path)
			kfree(path);
	}

	return retval;
}

static int
fmon_symlink(struct inode *i, struct dentry *d, const char *name)
{
	struct event *e;
	int retval = -1;
	char *path;

	if (fmon->linux_symlink) {
		retval = fmon->linux_symlink(i, d, name);
		if (!retval) {
			path = dentry_full_path(d);
			if (path && is_active) {
				e = create_event(E_SYM);
				strncpy(e->path_1, path, PATH_MAX);
				strncpy(e->path_2, name, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "symlink [%d] --> %s to %s\n", current->pid, path, name);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
fmon_mkdir(struct inode *i, struct dentry *d, int mode)
{
	struct event *event;
	int retval = -1;
	char *path;
	
	if (fmon->linux_mkdir) {
		retval = fmon->linux_mkdir(i, d, mode);
		if (!retval && is_active) {
			path = dentry_full_path(d);
			if (path) {
				event = create_event(E_MKD);

				strncpy(event->path_1, path, PATH_MAX);
				event->uid = i->i_uid;
				event->gid = i->i_gid;
				event->mode = mode;

				list_add(&event->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "mkdir [%d] --> %s mode: %d\n", current->pid, path, mode);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
fmon_rmdir(struct inode *i, struct dentry *d)
{
	struct event *e;
	int retval = -1;
	char *path;
	
	if (fmon->linux_rmdir) {
		/* obtain full path before rmdir */
		path = dentry_full_path(d);
		retval = fmon->linux_rmdir(i, d);
		if (!retval && is_active) {
			if (path) {
				e = create_event(E_RMD);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "rmdir [%d] --> %s\n", current->pid, path);
			}
		}

		kfree(path);
	}
	
	return retval;
}

static int
fmon_mknod(struct inode *i, struct dentry *d, int mode, dev_t dev)
{
	struct event *event;
	int retval = -1;
	char *path;

	if (fmon->linux_mknod) {
		retval = fmon->linux_mknod(i, d, mode, dev);
		if (!retval && is_active) {
			path = dentry_full_path(d);
			if (path) {
				event = create_event(E_MKN);
				strncpy(event->path_1, path, PATH_MAX);
				event->mode = mode;
				list_add(&event->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "mknod [%d] --> %s\n", current->pid, path);
			}
		}
	}
	
	return retval;
}

static int
fmon_rename(struct inode *i_1, struct dentry *d_1, struct inode *i_2, struct dentry *d_2)
{
	struct event *e;
	int retval = -1;
	char *path_d1 = NULL;
	char *path_d2 = NULL;

	if (fmon->linux_rename) {
		if (is_active) {
			path_d1 = dentry_full_path(d_1);
			path_d2 = dentry_full_path(d_2);
		}

		retval = fmon->linux_rename(i_1, d_1, i_2, d_2);
		if (!retval && is_active) {
			if (path_d1 && path_d2) {
				e = create_event(E_REN);

				strncpy(e->path_1, path_d1, PATH_MAX);
				strncpy(e->path_2, path_d2, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
				fmon->event_list_len++;

				printk(KERN_ERR "rename [%d] --> %s to %s\n", current->pid, path_d1, path_d2);
				kfree(path_d1);
				kfree(path_d2);
			}
		}
	}

	return retval;
}

static ssize_t
fmon_write(struct file *f, const char __user *user, size_t size, loff_t *offset)
{
	ssize_t ret = -1;
	char *d_path = NULL;

	/*
	 * Don't do here any printk, because
	 * of infinity loop.
	 */
	if (fmon->linux_write) {
		d_path = dentry_full_path(f->f_path.dentry);
		ret = fmon->linux_write(f, user, size, offset);
		fmon->event_list_len++;	/* just in order to check */

		if (d_path)
			kfree(d_path);
	}

	return ret;
}

static ssize_t
fmon_read(struct file *f, char __user *user, size_t size, loff_t *offset)
{
	ssize_t ret = -1;
	
	return ret;
}

int
assign_iop_ifop_opr(struct file_monitor *f, struct super_block *s)
{
	struct inode_operations *iop = NULL;
	struct file_operations *fop = NULL;
	struct dentry *tmp = NULL;

	/* skip bad stuff */
	if (s == NULL || f == NULL)
		goto fail;

	tmp = create_dentry(s, ".fmon");
	if (tmp && tmp->d_inode) {
		iop = (struct inode_operations *) s->s_root->d_inode->i_op;
		fop = (struct file_operations *) tmp->d_inode->i_fop;

		if (fop) {
			f->linux_write = fop->write;
			fop->write = f->linux_write ? fmon_write : NULL;
		}

		if (iop) {
			f->linux_create = iop->create;
			f->linux_unlink = iop->unlink;
			f->linux_link = iop->link;
			f->linux_symlink = iop->symlink;
			f->linux_mkdir = iop->mkdir;
			f->linux_rmdir = iop->rmdir;
			f->linux_mknod = iop->mknod;
			f->linux_rename = iop->rename;

			/* assign our own functions */
			iop->create = f->linux_create ? fmon_create : NULL;
			iop->unlink = f->linux_unlink ? fmon_unlink : NULL;
			iop->link = f->linux_link ? fmon_link : NULL;
			iop->symlink = f->linux_symlink ? fmon_symlink : NULL;
			iop->mkdir = f->linux_mkdir ? fmon_mkdir : NULL;
			iop->rmdir = f->linux_rmdir ? fmon_rmdir : NULL;
			iop->mknod = f->linux_mknod ? fmon_mknod : NULL;
			iop->rename = f->linux_rename ? fmon_rename : NULL;
		}

		/*
		 * here are some problems with unlinking i guess,
		 * because while next inserting it fails on
		 * create_dentry routine.
		 */
		unlink_dentry(s->s_root->d_inode, tmp);
		return 1;
	}

fail:
	return -1;
}

int
restore_iop_ifop_opr(struct file_monitor *f)
{
	struct inode_operations *iop = NULL;
	struct inode *i = NULL;

	if (f == NULL)
		return -1;

	i = f->sb->s_root->d_inode;
	iop = (struct inode_operations *) i->i_op;

	/* restore inode operations */
	iop->create = f->linux_create;
	iop->unlink = f->linux_unlink;
	iop->link = f->linux_link;
	iop->symlink = f->linux_symlink;
	iop->mkdir = f->linux_mkdir;
	iop->rmdir = f->linux_rmdir;
	iop->mknod = f->linux_mknod;
	iop->rename = f->linux_rename;

	/* restore file operations */
	/* TODO: XXX */

	return 1;
}

