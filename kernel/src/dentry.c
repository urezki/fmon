#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>

/* local headers */
#include <dentry.h>

/*
 * This routine is looking for vfsmount structure
 * that contains a dentry that is in question.
 */
struct vfsmount *
dentry_to_vfs(struct dentry *de)
{
	struct vfsmount		*dmnt		   = NULL;
	struct vfsmount		*rootmnt	   = NULL;
	struct dentry		*tmp		   = NULL;
	struct list_head	*vfsmount_list = NULL;
	struct fs_struct	*f			   = NULL;

	/* find root dentry of the mounted tree */
	for (tmp = de; tmp->d_parent != tmp; tmp = tmp->d_parent)
		;

	f = init_task.fs;

	read_lock(&f->lock);
	rootmnt = mntget(f->root.mnt);
	read_unlock(&f->lock);

	list_for_each(vfsmount_list, &rootmnt->mnt_list) {
		struct vfsmount *droot = list_entry(vfsmount_list, struct vfsmount, mnt_list);
		if (droot->mnt_root == tmp) {
			dmnt = mntget(droot);
			break;
		}
	}
	
	mntput(rootmnt);
	return dmnt ? dmnt:mntget(rootmnt);
}

/*
 * Goes through the dcache and builds absolute
 * path to the dentry that is in question.
 */
char *
dentry_full_path(struct dentry *d)
{
	struct vfsmount *d_mnt;
	char *full_path_name;

	if (d == NULL)
		goto out;
	
	d_mnt = dentry_to_vfs(d);
	if (d_mnt) {
		struct vfsmount *rootmnt;
		struct fs_struct *f = init_task.fs;
		char *end;
		
		/* PATH_MAX is 4096 including nul */
		full_path_name = kmalloc(sizeof(char) * PATH_MAX, GFP_KERNEL);
		memset(full_path_name, 0, PATH_MAX);
		
		read_lock(&f->lock);
		rootmnt = mntget(f->root.mnt);
		read_unlock(&f->lock);

		end = full_path_name + (PATH_MAX - 1);
		*end = '\0';

		if (d->d_inode)
			if (S_ISDIR(d->d_inode->i_mode))
				*--end = '/';
			
		while (1) {
			if (IS_ROOT(d)) {
				if (d_mnt == rootmnt)
					break;		/* root... give up */
				
				d = d_mnt->mnt_mountpoint;
				d_mnt = d_mnt->mnt_parent;
			}

			strncpy(end - d->d_name.len, d->d_name.name, d->d_name.len);
			end -= d->d_name.len;
			*--end = '/';
			
			d = d->d_parent;
		}

		mntput(rootmnt);
		return strncpy(full_path_name, end, PATH_MAX - 1);
	}
	
out:
	return NULL;
}
