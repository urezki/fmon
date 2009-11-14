#ifndef __INODE_H__
#define __INODE_H__

extern int restore_iop_ifop_opr(struct file_monitor *);
extern int assign_iop_ifop_opr(struct file_monitor *, struct super_block *);

extern int is_active;

#endif	/* __INODE_H__ */
