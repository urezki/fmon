#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mount.h>
#include <linux/string.h>

struct super_block*
vfsmount_mnt_sb(struct vfsmount *v)
{
	return v->mnt_sb;
}

struct super_block*
fmon_get_super(struct super_block *sb)
{
	return get_super(sb->s_bdev);
}

struct fs_struct*
init_task_struct_fs(void)
{
	return init_task.fs;
}

struct vfsmount*
fs_struct_rootmnt(struct fs_struct *f)
{
	return f->root.mnt;
}

struct super_block*
find_init_superblock(void)
{
	struct vfsmount *rootmnt;
	struct super_block *sb;
	struct fs_struct *init_fs;

	init_fs = init_task_struct_fs();
	rootmnt = mntget(fs_struct_rootmnt(init_fs));
	sb = fmon_get_super(vfsmount_mnt_sb(rootmnt));

	mntput(rootmnt);
	return sb;
}
