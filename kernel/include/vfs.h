#ifndef __VFS_H__
#define __VFS_H__

struct super_block *vfsmount_mnt_sb(struct vfsmount *v);
struct super_block *fmon_get_super(struct super_block *sb);
struct vfsmount *fs_struct_rootmnt(struct fs_struct *fs);
struct super_block *find_init_superblock(void);
struct fs_struct *init_task_struct_fs(void);

#endif	/* __VFS_H__ */
