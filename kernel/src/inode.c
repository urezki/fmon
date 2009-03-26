#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp_lock.h>

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>

#define NFS_MAGIC 0x6969

static struct inode_operations *nfs_iop;
static struct file_operations *nfs_fop;

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

static int
my_create(struct inode *inode, struct dentry *dentry, int foo, struct nameidata *namei)
{
	int retval = -1;
	char *path;
	
	if (linux_create) {
		retval = linux_create(inode, dentry, foo, namei);
		if (!retval) {
			path = dentry_full_path(dentry);
			if (path) {
				printk(KERN_ERR "create [%d] --> %s\n", current->pid, path);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static struct dentry *
my_lookup(struct inode *inode, struct dentry *dentry, struct nameidata *namei)
{
	struct dentry *d = NULL;

	if (linux_lookup)
		d = linux_lookup(inode, dentry, namei);
	
	return d;
}

static int
my_link(struct dentry *d_1, struct inode *i, struct dentry *d_2)
{
	int retval = -1;
	char *path_d1;
	char *path_d2;
	
	if (linux_link) {
		retval = linux_link(d_1, i, d_2);
		if (!retval) {
			path_d1 = dentry_full_path(d_1);
			path_d2 = dentry_full_path(d_2);

			if (path_d1 && path_d2) {
				printk(KERN_ERR "link [%d] --> %s to %s\n", current->pid, path_d1, path_d2);
				kfree(path_d1);
				kfree(path_d2);
			}
		}
	}
	
	return retval;
}

static int
my_unlink(struct inode *inode, struct dentry *dentry)
{
	int retval = -1;
	char *path;
	
	if (linux_unlink) {
		path = dentry_full_path(dentry);
		retval = linux_unlink(inode, dentry);
		if (!retval) {
			if (path)
				printk(KERN_ERR "unlink [%d] --> %s\n", current->pid, path);
		}
		
		if (path)
			kfree(path);
	}
	
	return retval;
}

static int
my_symlink(struct inode *i, struct dentry *d, const char *name)
{
	int retval = -1;
	char *path;
	
	if (linux_symlink) {
		retval = linux_symlink(i, d, name);
		if (!retval) {
			path = dentry_full_path(d);
			if (path) {
				printk(KERN_ERR "symlink [%d] --> %s to %s\n", current->pid, path, name);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
my_mkdir(struct inode *i, struct dentry *d, int mode)
{
	int retval = -1;
	char *path;
	
	if (linux_mkdir) {
		retval = linux_mkdir(i, d, mode);
		if (!retval) {
			path = dentry_full_path(d);
			if (path) {
				printk(KERN_ERR "mkdir [%d] --> %s\n", current->pid, path);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
my_rmdir(struct inode *i, struct dentry *d)
{
	int retval = -1;
	char *path;
	
	if (linux_rmdir) {
		retval = linux_rmdir(i, d);
		if (!retval) {
			path = dentry_full_path(d);
			if (path) {
				printk(KERN_ERR "rmdir [%d] --> %s\n", current->pid, path);
				kfree(path);
			}
		}
	}
	
	return retval;
}

static int
my_mknod(struct inode *i, struct dentry *d, int mode, dev_t dev)
{
	int retval = -1;

	if (linux_mknod)
		retval = linux_mknod(i, d, mode, dev);
	
	return retval;
}

static int
my_rename(struct inode *i_1, struct dentry *d_1, struct inode *i_2, struct dentry *d_2)
{
	int retval = -1;
	char *path_d1;
	char *path_d2;
	
	if (linux_rename) {
		path_d1 = dentry_full_path(d_1);
		path_d2 = dentry_full_path(d_2);
		retval = linux_rename(i_1, d_1, i_2, d_2);
		if (!retval) {
			if (path_d1 && path_d2) {
				printk(KERN_ERR "rename [%d] --> %s to %s\n", current->pid, path_d1, path_d2);
				kfree(path_d1);
				kfree(path_d2);
			}
		}
	}
	
	return retval;
}

static int
my_readlink(struct dentry *d, char __user *data, int mode)
{
	int retval = -1;

	if (linux_readlink)
		retval = linux_readlink(d, data, mode);
	
	return retval;
}

static void *
my_follow_link(struct dentry *d, struct nameidata *namei)
{
	void *retval = NULL;

	if (linux_follow_link)
		retval = linux_follow_link(d, namei);
	
	return retval;
}

static void
my_put_link(struct dentry *d, struct nameidata *namei, void *data)
{
	if (linux_put_link)
		linux_put_link(d, namei, data);
}

static void
my_truncate(struct inode *i)
{
	if (linux_truncate)
		linux_truncate(i);
}

static int
my_permission(struct inode *i, int mode, struct nameidata *namei)
{
	int retval = -1;

	if (linux_permission)
		retval = linux_permission(i, mode, namei);

	return retval;
}

static int
my_setattr(struct dentry *d, struct iattr *attr)
{
	int retval = -1;

	if (linux_setattr)
		retval = linux_setattr(d, attr);
	
	return retval;
}

static int
my_getattr(struct vfsmount *mnt, struct dentry *d, struct kstat *k)
{
	int retval = -1;
	
	if (linux_getattr)
		retval = linux_getattr(mnt, d, k);

	return retval;
}

static int
my_setxattr(struct dentry *d, const char *name, const void *data, size_t len, int a)
{
	int retval = -1;

	if (linux_setxattr)
		retval = linux_setxattr(d, name, data, len, a);

	return retval;
}

static ssize_t
my_getxattr(struct dentry *d, const char *name, void *data, size_t len)
{
	ssize_t retval = -1;

	if (linux_getxattr)
		retval = linux_getxattr(d, name, data, len);
	
	return retval;
}

static ssize_t
my_listxattr(struct dentry *d, char *name, size_t size)
{
	ssize_t retval = -1;

	if (linux_listxattr)
		retval = linux_listxattr(d, name, size);
	
	return retval;
}

static int
my_removexattr(struct dentry *d, const char *name)
{
	int retval = -1;

	if (linux_removexattr)
		retval = linux_removexattr(d, name);
	
	return retval;
}

static void
my_truncate_range(struct inode *i, loff_t off_1, loff_t off_2)
{
	if (linux_truncate_range)
		linux_truncate_range(i, off_1, off_2);
}

static long
my_fallocate(struct inode *i, int mode, loff_t offset, loff_t len)
{
	long retval = -1;

	if (linux_fallocate)
		retval = linux_fallocate(i, mode, offset, len);

	return retval;
}

asmlinkage struct super_block*
vfsmount_mnt_sb(struct vfsmount *v)
{
	return v->mnt_sb;
}

asmlinkage struct super_block*
cd_get_super(struct super_block *sb)
{
	return get_super(sb->s_bdev);
}

asmlinkage struct fs_struct*
init_task_struct_fs(void)
{
	return init_task.fs;
}

asmlinkage struct vfsmount*
fs_struct_rootmnt(struct fs_struct *f)
{
	return f->root.mnt;
}

static struct super_block*
find_init_superblock(void)
{
	struct vfsmount *rootmnt;
	struct super_block *sb;
	struct fs_struct *init_fs;
	
	init_fs = init_task_struct_fs();
	printk(KERN_ERR "init_fs is: %p\n", init_fs);
	
	rootmnt = mntget(fs_struct_rootmnt(init_fs));
	printk(KERN_ERR "rootmnt is: %p\n", rootmnt);

	sb = cd_get_super(vfsmount_mnt_sb(rootmnt));
	printk(KERN_ERR "sb is: %p\n", sb);
		
	mntput(rootmnt);
	return sb;
}

static int __init
d_nfs_init (void)
{
	struct super_block	*init_sb	= NULL;
	struct task_struct	*init		= NULL;
	struct list_head	*list_sb	= NULL;
	struct vfsmount		*root_mnt	= NULL;
	struct dentry		*nfs_dentry = NULL;
	struct inode		*nfs_inode	= NULL;
	struct super_block	*nfs_sb		= NULL;


	printk(KERN_ERR "starting !!!\n");
	
	init = &init_task;
	root_mnt = init->fs->root.mnt;
	init_sb = find_init_superblock();

	list_for_each(list_sb, &init_sb->s_list) {
		nfs_sb = list_entry(list_sb, struct super_block, s_list);
		
		if (nfs_sb && nfs_sb->s_magic == NFS_MAGIC) {
			struct hlist_node *lp = NULL;
			
			hlist_for_each(lp, &nfs_sb->s_anon) {
				nfs_dentry = hlist_entry(lp, struct dentry, d_hash);
			}
			
			break;
		}
	}

	if (nfs_dentry) {
		nfs_inode = nfs_dentry->d_inode;
		if (nfs_inode) {
			nfs_iop = (struct inode_operations *) nfs_inode->i_op;
			nfs_fop = (struct file_operations *) nfs_inode->i_fop;
			if (nfs_iop && nfs_fop) {
				printk(KERN_ERR "saving whole inode operations...\n");
				linux_create = nfs_iop->create;
				linux_unlink = nfs_iop->unlink;
				linux_lookup = nfs_iop->lookup;
				linux_link = nfs_iop->link;
				linux_symlink = nfs_iop->symlink;
				linux_mkdir = nfs_iop->mkdir;
				linux_rmdir = nfs_iop->rmdir;
				linux_mknod = nfs_iop->mknod;
				linux_rename = nfs_iop->rename;
				linux_readlink = nfs_iop->readlink;
				linux_follow_link = nfs_iop->follow_link;
				linux_put_link = nfs_iop->put_link;
				linux_truncate = nfs_iop->truncate;
				linux_permission = nfs_iop->permission;
				linux_setattr = nfs_iop->setattr;
				linux_getattr = nfs_iop->getattr;
				linux_setxattr = nfs_iop->setxattr;
				linux_getxattr = nfs_iop->getxattr;
				linux_listxattr = nfs_iop->listxattr;
				linux_removexattr = nfs_iop->removexattr;
				linux_truncate_range = nfs_iop->truncate_range;
				linux_fallocate = nfs_iop->fallocate;
				
				printk(KERN_ERR "assigning our own functions...\n");
				nfs_iop->create = linux_create ? my_create : NULL;
				nfs_iop->unlink = linux_unlink ? my_unlink : NULL;
				nfs_iop->lookup = linux_lookup ? my_lookup : NULL;
				nfs_iop->link = linux_link ? my_link : NULL;
				nfs_iop->symlink = linux_symlink ? my_symlink : NULL;
				nfs_iop->mkdir = linux_mkdir ? my_mkdir : NULL;
				nfs_iop->rmdir = linux_rmdir ? my_rmdir : NULL;
				nfs_iop->mknod = linux_mknod ? my_mknod : NULL;
				nfs_iop->rename = linux_rename ? my_rename : NULL;
				nfs_iop->readlink = linux_readlink ? my_readlink : NULL;
				nfs_iop->follow_link = linux_follow_link ? my_follow_link : NULL;
				nfs_iop->put_link = linux_put_link ?  my_put_link : NULL;
				nfs_iop->truncate = linux_truncate ? my_truncate : NULL;
				nfs_iop->permission = linux_permission ? my_permission : NULL;
				nfs_iop->setattr = linux_setattr ? my_setattr : NULL;
				nfs_iop->getattr = linux_getattr ? my_getattr : NULL;
				nfs_iop->setxattr = linux_setxattr ? my_setxattr : NULL;
				nfs_iop->getxattr = linux_getxattr ? my_getxattr : NULL;
				nfs_iop->listxattr = linux_listxattr ? my_listxattr : NULL;
				nfs_iop->removexattr = linux_removexattr ? my_removexattr : NULL;
				nfs_iop->truncate_range = linux_truncate_range ? my_truncate_range : NULL;
				nfs_iop->fallocate = linux_fallocate ? my_fallocate : NULL;
			}
		}
	}
	
	/* { */
	/* 	struct vfsmount *nfs_mnt; */
		
	/* 	nfs_mnt = dentry_to_vfs(nfs_dentry); */
	/* 	if (nfs_mnt) { */
	/* 		if (nfs_mnt->mnt_sb->s_export_op) */
	/* 			printk(KERN_ERR "not NULL !!!\n"); */
	/* 		else */
	/* 			printk(KERN_ERR "NULL !!!\n"); */

	/* 		if (nfs_mnt->mnt_parent == init_task.fs->root.mnt) */
	/* 			printk(KERN_ERR "--> %p\n", nfs_mnt->mnt_parent); */

	/* 		printk(KERN_ERR "YES !!! %p\n", nfs_mnt->mnt_root); */
			
			
	/* 		printk(KERN_ERR "--> %s -- %s\n", nfs_mnt->mnt_parent->mnt_mountpoint->d_name.name, nfs_mnt->mnt_mountpoint->d_name.name); */
	/* 		printk(KERN_ERR "--> %s%s%s\n", nfs_mnt->mnt_mountpoint->d_name.name, */
	/* 			   nfs_mnt->mnt_mountpoint->d_parent->d_name.name, nfs_mnt->mnt_mountpoint->d_parent->d_parent->d_name.name); */
			
			
	/* 		mntput(nfs_mnt); */
	/* 	} */
	/* } */

	/* { */
	/* 	struct dentry *new_dentry; */
	/* 	struct dentry *real_dentry; */
	/* 	struct qstr name; */
	/* 	int err; */
		
	/* 	name.name = "vm"; */
	/* 	name.len = strlen(name.name); */
	/* 	name.hash = full_name_hash(name.name, name.len); */

	/* 	real_dentry = d_lookup(nfs_dentry, &name); */
	/* 	if (real_dentry) { */
	/* 		struct dentry *chld; */
	/* 		printk(KERN_ERR "1) found %p -- %p -- %p\n", real_dentry, real_dentry->d_parent, real_dentry->d_parent->d_parent); */
			
	/* 		name.name = "foo"; */
	/* 		name.len = strlen(name.name); */
	/* 		name.hash = full_name_hash(name.name, name.len); */
			
	/* 		chld = d_lookup(real_dentry, &name); */
	/* 		if (chld) { */
	/* 			printk(KERN_ERR "2) found %p -- %p -- %p\n", chld, chld->d_parent, chld->d_parent->d_parent); */
	/* 			dput(chld); */
	/* 		} */
	/* 		dput(real_dentry); */
	/* 	} else { */
	/* 	  	/\* new_dentry = d_alloc_name(nfs_dentry, ".cdi"); *\/ */
	/* 	  	/\* if (new_dentry) { *\/ */
	/* 	  	/\* 	struct nameidata nd; *\/ */
				
	/* 	  	/\* 	memset(&nd, 0, sizeof(nd)); *\/ */
	/* 	  	/\* 	printk(KERN_ERR "allocated %p\n", new_dentry); *\/ */
				
	/* 	  	/\* 	vfs_create(nfs_dentry->d_inode, new_dentry, S_IFREG|S_IRWXU, &nd); *\/ */
				
	/* 	  	/\* 	name.name = ".cdi"; *\/ */
	/* 	  	/\* 	name.len = strlen(name.name); *\/ */
	/* 	  	/\* 	name.hash = full_name_hash(name.name, name.len); *\/ */
				
	/* 	  	/\* 	real_dentry = d_lookup(nfs_dentry, &name); *\/ */
	/* 	  	/\* 	if (real_dentry) { *\/ */
	/* 	  	/\* 		printk(KERN_ERR "!!! Found !!! %s\n", name.name); *\/ */
	/* 	  	/\* 		dput(real_dentry); *\/ */
	/* 	  	/\* 	} *\/ */
	/* 	  	/\* } *\/ */
	/* 	} */
	/* } */

	{
		struct qstr name;
		struct dentry *d;
		struct vfsmount *nfs_mnt;
		
		nfs_mnt = dentry_to_vfs(nfs_dentry);
		if (nfs_mnt) {
			struct vfsmount *parent = nfs_mnt->mnt_parent;

			if (parent && parent->mnt_sb->s_export_op) {
				d = parent->mnt_sb->s_export_op->get_parent(nfs_mnt->mnt_mountpoint);
				if (d)
					printk(KERN_ERR "my congratulation, we found it !!!!! %s\n", d->d_name.name);
			}
		}
		
		name.name = "1";
		name.len = strlen(name.name);
		name.hash = full_name_hash(name.name, name.len);;
		
		dget(nfs_dentry);
		d = d_lookup(nfs_dentry, &name);
		if (d) {
			struct dentry *foo;
			struct qstr name_foo;

			name_foo.name = "foo";
			name_foo.len = strlen(name_foo.name);
			name_foo.hash = full_name_hash(name_foo.name, name_foo.len);;
			
			printk(KERN_ERR "found %s -- %d -- %s\n", d->d_name.name, d->d_count.counter, d->d_parent->d_name.name);
			
			foo = d_lookup(d, &name_foo);
			if (foo) {
				struct vfsmount *nfs_mnt;

				printk(KERN_ERR "found %s -- %d\n", foo->d_name.name, foo->d_count.counter);

				nfs_mnt = dentry_to_vfs(nfs_dentry);
				if (nfs_mnt) {
					char buf[4096] = {'\0'};
					char *ret;
					int retval;
					
					retval = find_name(nfs_mnt, d, buf, foo);
					printk(KERN_ERR "------------------> %d\n", retval);
					
					ret = dentry_full_path(foo);
					printk(KERN_ERR "---> full path is: %s\n", ret);

					mntput(nfs_mnt);
				}

				dput(foo);
			}

			dput(d);
		}

		if (IS_ROOT(nfs_dentry))
			printk(KERN_ERR "root %p %p!!!\n", nfs_dentry, nfs_dentry->d_parent);

		dput(nfs_dentry);
	}

	return 0;
}

static void __exit
d_nfs_exit (void)
{
	printk(KERN_ERR "stopping !!!\n");
	
	if (nfs_iop) {
		printk(KERN_ERR "reassigning our own functions...\n");
		nfs_iop->create = linux_create;
		nfs_iop->unlink = linux_unlink;
		nfs_iop->lookup = linux_lookup;
		nfs_iop->link = linux_link;
		nfs_iop->symlink = linux_symlink;
		nfs_iop->mkdir = linux_mkdir;
		nfs_iop->rmdir = linux_rmdir;
		nfs_iop->mknod = linux_mknod;
		nfs_iop->rename = linux_rename;
		nfs_iop->readlink = linux_readlink;
		nfs_iop->follow_link = linux_follow_link;
		nfs_iop->put_link = linux_put_link;
		nfs_iop->truncate = linux_truncate;
		nfs_iop->permission = linux_permission;
		nfs_iop->setattr = linux_setattr;
		nfs_iop->getattr = linux_getattr;
		nfs_iop->setxattr = linux_setxattr;
		nfs_iop->getxattr = linux_getxattr;
		nfs_iop->listxattr = linux_listxattr;
		nfs_iop->removexattr = linux_removexattr;
		nfs_iop->truncate_range = linux_truncate_range;
		nfs_iop->fallocate = linux_fallocate;
	}
}

module_init(d_nfs_init);
module_exit(d_nfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Uladzislau Rezki");
