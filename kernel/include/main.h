#ifndef __MAIN_H__
#define __MAIN_H__

struct file_monitor {
	struct list_head list;		/* Keep this first */
	struct super_block *sb;

	/* inode operations*/
	int (*linux_create)(struct inode *,struct dentry *,int, struct nameidata *);
	int (*linux_link)(struct dentry *,struct inode *,struct dentry *);
	int (*linux_unlink)(struct inode *,struct dentry *);
	int (*linux_symlink)(struct inode *,struct dentry *,const char *);
	int (*linux_mkdir)(struct inode *,struct dentry *,int);
	int (*linux_rmdir)(struct inode *,struct dentry *);
	int (*linux_mknod)(struct inode *,struct dentry *,int,dev_t);
	int (*linux_rename)(struct inode *, struct dentry *, struct inode *, struct dentry *);

	/* file operations */
	ssize_t (*linux_write)(struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*linux_read)(struct file *, char __user *, size_t, loff_t *);

	struct list_head event_list; /* all events */
	unsigned int event_list_len;
};

extern struct file_monitor *fmon;
extern struct list_head fmon_list;

#endif	/* __MAIN_H__ */
