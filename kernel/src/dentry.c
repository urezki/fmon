#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>
#include <linux/version.h>

/* local headers */
#include <dentry.h>
#include <str.h>

struct dentry *
create_dentry(struct super_block *s, const char *name)
{
	struct dentry *new_dentry;
	struct nameidata nd;
	struct qstr *q;
	int err;

	if (!s || !name)
		goto out;

	q = make_qstr(name, strlen(name), 0);
	new_dentry = d_alloc(s->s_root, q);
	free_qstr(q);				/* free it */

	if (new_dentry) {
		memset(&nd, 0, sizeof(nd));

		nd.path.dentry = s->s_root;
		nd.path.mnt = current->fs->root.mnt;

		err = vfs_create(s->s_root->d_inode, new_dentry, S_IFREG|S_IRWXU, &nd);
		if (err) {
			printk(KERN_ERR "vfs_create error: %s:%d\n", __FUNCTION__, __LINE__);
			goto out;
		}

		return new_dentry;
	}

out:
	return NULL;
}

int
unlink_dentry(struct inode *dir, struct dentry *victim)
{
	if (!dir || !victim)
		goto out;

	return vfs_unlink(dir, victim);
out:
	return -1;
}

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
	
	/* PATH_MAX is 4096 including nul */
	full_path_name = kmalloc(sizeof(char) * PATH_MAX, GFP_KERNEL);
	memset(full_path_name, 0, PATH_MAX);

	d_mnt = dentry_to_vfs(d);
	if (d_mnt) {
		struct vfsmount *rootmnt;
		struct fs_struct *f = init_task.fs;
		char *end;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
		{
			struct path p = { .mnt = d_mnt, .dentry = d };
			char buf[PATH_MAX] = {'\0'};
			char *path;

			path = d_path(&p, buf, PATH_MAX);
			return strncpy(full_path_name, path, PATH_MAX);
		}
#endif
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
					break;		/* global root, give up */
				
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
