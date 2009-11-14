#ifndef __DENTRY_H__
#define __DENTRY_H__

extern struct vfsmount *dentry_to_vfs(struct dentry *);
extern char *dentry_full_path(struct dentry *);
extern struct dentry *create_dentry(struct super_block *, const char *);
extern int unlink_dentry(struct inode *, struct dentry *);

#endif	/* __DENTRY_H__ */
