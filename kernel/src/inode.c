#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>

/* local headers */
#include <dentry.h>
#include <event.h>
#include <main.h>

static int
fmon_create(struct inode *inode, struct dentry *dentry, int foo, struct nameidata *namei)
{
	struct event *e = NULL;
	int retval = -1;
	char *path;
	
	if (fmon->linux_create) {
		retval = fmon->linux_create(inode, dentry, foo, namei);
		if (!retval) {
			path = dentry_full_path(dentry);
			if (path) {
				e = create_event(CREATE_EVENT);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

				printk(KERN_ERR "create [%d] --> %s\n", current->pid, path);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static struct dentry *
fmon_lookup(struct inode *inode, struct dentry *dentry, struct nameidata *namei)
{
	struct dentry *d = NULL;

	if (fmon->linux_lookup)
		d = fmon->linux_lookup(inode, dentry, namei);
	
	return d;
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
		if (!retval) {
			path_d1 = dentry_full_path(d_1);
			path_d2 = dentry_full_path(d_2);

			if (path_d1 && path_d2) {
				e = create_event(LINK_EVENT);
				strncpy(e->path_1, path_d1, PATH_MAX);
				strncpy(e->path_2, path_d2, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

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
		path = dentry_full_path(dentry);
		retval = fmon->linux_unlink(inode, dentry);
		if (!retval) {
			if (path) {
				e = create_event(UNLINK_EVENT);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);
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
			if (path) {
				e = create_event(SYMLINK_EVENT);
				strncpy(e->path_1, path, PATH_MAX);
				strncpy(e->path_2, name, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

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
	struct event *e;
	int retval = -1;
	char *path;
	
	if (fmon->linux_mkdir) {
		retval = fmon->linux_mkdir(i, d, mode);
		if (!retval) {
			path = dentry_full_path(d);
			if (path) {
				e = create_event(MKDIR_EVENT);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

				printk(KERN_ERR "mkdir [%d] --> %s\n", current->pid, path);
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
		retval = fmon->linux_rmdir(i, d);
		if (!retval) {
			path = dentry_full_path(d);
			if (path) {
				e = create_event(RMDIR_EVENT);
				strncpy(e->path_1, path, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

				printk(KERN_ERR "rmdir [%d] --> %s\n", current->pid, path);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
fmon_mknod(struct inode *i, struct dentry *d, int mode, dev_t dev)
{
	int retval = -1;

	if (fmon->linux_mknod)
		retval = fmon->linux_mknod(i, d, mode, dev);
	
	return retval;
}

static int
fmon_rename(struct inode *i_1, struct dentry *d_1, struct inode *i_2, struct dentry *d_2)
{
	struct event *e;
	int retval = -1;
	char *path_d1;
	char *path_d2;
	
	if (fmon->linux_rename) {
		path_d1 = dentry_full_path(d_1);
		path_d2 = dentry_full_path(d_2);
		retval = fmon->linux_rename(i_1, d_1, i_2, d_2);
		if (!retval) {
			if (path_d1 && path_d2) {
				e = create_event(RENAME_EVENT);

				strncpy(e->path_1, path_d1, PATH_MAX);
				strncpy(e->path_2, path_d2, PATH_MAX);
				list_add(&e->list, &fmon->event_list);

				printk(KERN_ERR "rename [%d] --> %s to %s\n", current->pid, path_d1, path_d2);
				kfree(path_d1);
				kfree(path_d2);
			}
		}
	}
	
	return retval;
}

static int
fmon_readlink(struct dentry *d, char __user *data, int mode)
{
	int retval = -1;

	if (fmon->linux_readlink)
		retval = fmon->linux_readlink(d, data, mode);
	
	return retval;
}

static void *
fmon_follow_link(struct dentry *d, struct nameidata *namei)
{
	void *retval = NULL;

	if (fmon->linux_follow_link)
		retval = fmon->linux_follow_link(d, namei);
	
	return retval;
}

static void
fmon_put_link(struct dentry *d, struct nameidata *namei, void *data)
{
	if (fmon->linux_put_link)
		fmon->linux_put_link(d, namei, data);
}

static void
fmon_truncate(struct inode *i)
{
	if (fmon->linux_truncate)
		fmon->linux_truncate(i);
}

static int
fmon_permission(struct inode *i, int mode, struct nameidata *namei)
{
	int retval = -1;

	if (fmon->linux_permission)
		retval = fmon->linux_permission(i, mode, namei);

	return retval;
}

static int
fmon_setattr(struct dentry *d, struct iattr *attr)
{
	int retval = -1;

	if (fmon->linux_setattr)
		retval = fmon->linux_setattr(d, attr);
	
	return retval;
}

static int
fmon_getattr(struct vfsmount *mnt, struct dentry *d, struct kstat *k)
{
	int retval = -1;
	
	if (fmon->linux_getattr)
		retval = fmon->linux_getattr(mnt, d, k);

	return retval;
}

static int
fmon_setxattr(struct dentry *d, const char *name, const void *data, size_t len, int a)
{
	int retval = -1;

	if (fmon->linux_setxattr)
		retval = fmon->linux_setxattr(d, name, data, len, a);

	return retval;
}

static ssize_t
fmon_getxattr(struct dentry *d, const char *name, void *data, size_t len)
{
	ssize_t retval = -1;

	if (fmon->linux_getxattr)
		retval = fmon->linux_getxattr(d, name, data, len);
	
	return retval;
}

static ssize_t
fmon_listxattr(struct dentry *d, char *name, size_t size)
{
	ssize_t retval = -1;

	if (fmon->linux_listxattr)
		retval = fmon->linux_listxattr(d, name, size);
	
	return retval;
}

static int
fmon_removexattr(struct dentry *d, const char *name)
{
	int retval = -1;

	if (fmon->linux_removexattr)
		retval = fmon->linux_removexattr(d, name);
	
	return retval;
}

static void
fmon_truncate_range(struct inode *i, loff_t off_1, loff_t off_2)
{
	if (fmon->linux_truncate_range)
		fmon->linux_truncate_range(i, off_1, off_2);
}

static long
fmon_fallocate(struct inode *i, int mode, loff_t offset, loff_t len)
{
	long retval = -1;

	if (fmon->linux_fallocate)
		retval = fmon->linux_fallocate(i, mode, offset, len);

	return retval;
}

int
assign_inode_op(struct file_monitor *f, struct dentry *s_root)
{
	struct inode_operations *iop = NULL;
	struct inode *i = NULL;
	
	/* skip bad stuff */
	if (!f || !s_root || s_root->d_parent != s_root)
		goto fail;

	i = s_root->d_inode;
	if (i) {
		iop = (struct inode_operations *) i->i_op;
		if (iop) {
			f->linux_create = iop->create;
			f->linux_unlink = iop->unlink;
			f->linux_lookup = iop->lookup;
			f->linux_link = iop->link;
			f->linux_symlink = iop->symlink;
			f->linux_mkdir = iop->mkdir;
			f->linux_rmdir = iop->rmdir;
			f->linux_mknod = iop->mknod;
			f->linux_rename = iop->rename;
			f->linux_readlink = iop->readlink;
			f->linux_follow_link = iop->follow_link;
			f->linux_put_link = iop->put_link;
			f->linux_truncate = iop->truncate;
			f->linux_permission = iop->permission;
			f->linux_setattr = iop->setattr;
			f->linux_getattr = iop->getattr;
			f->linux_setxattr = iop->setxattr;
			f->linux_getxattr = iop->getxattr;
			f->linux_listxattr = iop->listxattr;
			f->linux_removexattr = iop->removexattr;
			f->linux_truncate_range = iop->truncate_range;
			f->linux_fallocate = iop->fallocate;

			/* assign our own functions */
			iop->create = f->linux_create ? fmon_create : NULL;
			iop->unlink = f->linux_unlink ? fmon_unlink : NULL;
			iop->lookup = f->linux_lookup ? fmon_lookup : NULL;
			iop->link = f->linux_link ? fmon_link : NULL;
			iop->symlink = f->linux_symlink ? fmon_symlink : NULL;
			iop->mkdir = f->linux_mkdir ? fmon_mkdir : NULL;
			iop->rmdir = f->linux_rmdir ? fmon_rmdir : NULL;
			iop->mknod = f->linux_mknod ? fmon_mknod : NULL;
			iop->rename = f->linux_rename ? fmon_rename : NULL;
			iop->readlink = f->linux_readlink ? fmon_readlink : NULL;
			iop->follow_link = f->linux_follow_link ? fmon_follow_link : NULL;
			iop->put_link = f->linux_put_link ?  fmon_put_link : NULL;
			iop->truncate = f->linux_truncate ? fmon_truncate : NULL;
			iop->permission = f->linux_permission ? fmon_permission : NULL;
			iop->setattr = f->linux_setattr ? fmon_setattr : NULL;
			iop->getattr = f->linux_getattr ? fmon_getattr : NULL;
			iop->setxattr = f->linux_setxattr ? fmon_setxattr : NULL;
			iop->getxattr = f->linux_getxattr ? fmon_getxattr : NULL;
			iop->listxattr = f->linux_listxattr ? fmon_listxattr : NULL;
			iop->removexattr = f->linux_removexattr ? fmon_removexattr : NULL;
			iop->truncate_range = f->linux_truncate_range ? fmon_truncate_range : NULL;
			iop->fallocate = f->linux_fallocate ? fmon_fallocate : NULL;

			return 1;
		}
	}
	
fail:
	return -1;
}


int
reset_inode_op(struct file_monitor *f)
{
	struct inode_operations *iop = NULL;
	struct inode *i = NULL;
	
	if (f == NULL)
		return -1;

	i = f->sb->s_root->d_inode;
	iop = (struct inode_operations *) i->i_op;

	/* replace ... */
	iop->create = f->linux_create;
	iop->unlink = f->linux_unlink;
	iop->lookup = f->linux_lookup;
	iop->link = f->linux_link;
	iop->symlink = f->linux_symlink;
	iop->mkdir = f->linux_mkdir;
	iop->rmdir = f->linux_rmdir;
	iop->mknod = f->linux_mknod;
	iop->rename = f->linux_rename;
	iop->readlink = f->linux_readlink;
	iop->follow_link = f->linux_follow_link;
	iop->put_link = f->linux_put_link;
	iop->truncate = f->linux_truncate;
	iop->permission = f->linux_permission;
	iop->setattr = f->linux_setattr;
	iop->getattr = f->linux_getattr;
	iop->setxattr = f->linux_setxattr;
	iop->getxattr = f->linux_getxattr;
	iop->listxattr = f->linux_listxattr;
	iop->removexattr = f->linux_removexattr;
	iop->truncate_range = f->linux_truncate_range;
	iop->fallocate = f->linux_fallocate;

	return 1;
}
