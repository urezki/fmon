#ifndef __MAIN_H__
#define __MAIN_H__

struct file_monitor {
	struct list_head list;		/* Keep this first */
	struct super_block *sb;

	/* inode linux's own operations, for backup goals */
	int (*linux_create)(struct inode *,struct dentry *,int, struct nameidata *);
	struct dentry * (*linux_lookup)(struct inode *,struct dentry *, struct nameidata *);
	int (*linux_link)(struct dentry *,struct inode *,struct dentry *);
	int (*linux_unlink)(struct inode *,struct dentry *);
	int (*linux_symlink)(struct inode *,struct dentry *,const char *);
	int (*linux_mkdir)(struct inode *,struct dentry *,int);
	int (*linux_rmdir)(struct inode *,struct dentry *);
	int (*linux_mknod)(struct inode *,struct dentry *,int,dev_t);
	int (*linux_rename)(struct inode *, struct dentry *, struct inode *, struct dentry *);
	int (*linux_readlink)(struct dentry *, char __user *,int);
	void * (*linux_follow_link)(struct dentry *, struct nameidata *);
	void (*linux_put_link)(struct dentry *, struct nameidata *, void *);
	void (*linux_truncate)(struct inode *);
	int (*linux_permission)(struct inode *, int, struct nameidata *);
	int (*linux_setattr)(struct dentry *, struct iattr *);
	int (*linux_getattr)(struct vfsmount *mnt, struct dentry *, struct kstat *);
	int (*linux_setxattr)(struct dentry *, const char *,const void *,size_t,int);
	ssize_t (*linux_getxattr)(struct dentry *, const char *, void *, size_t);
	ssize_t (*linux_listxattr)(struct dentry *, char *, size_t);
	int (*linux_removexattr)(struct dentry *, const char *);
	void (*linux_truncate_range)(struct inode *, loff_t, loff_t);
	long (*linux_fallocate)(struct inode *inode, int mode, loff_t offset, loff_t len);

	struct list_head event_list; /* all events */
	unsigned int event_list_len;
};

extern struct file_monitor *fmon;
extern struct list_head fmon_list;

#endif	/* __MAIN_H__ */
