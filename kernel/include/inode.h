#ifndef __INODE_H__
#define __INODE_H__

extern int assign_inode_op(struct file_monitor *f, struct dentry *d);
extern int restore_inode_op(struct file_monitor *f);

extern int is_active;

#endif	/* __INODE_H__ */
