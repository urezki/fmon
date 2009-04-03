#ifndef __EVENT_H__
#define __EVENT_H__

enum {
	CREATE_EVENT, WRITE_EVENT, SYMLINK_EVENT, LINK_EVENT,
	UNLINK_EVENT, MKDIR_EVENT, RMDIR_EVENT, MKNOD_EVENT,
	RENAME_EVENT
};

struct event {
	struct list_head list;

	char path_1[PATH_MAX];
	char path_2[PATH_MAX];

	/* event type */
	int e_type;

	/* payload data */
	void *data;
	int data_size;

	/* permissions */
	uid_t uid;
	gid_t gid;
	int mode;

	/* actual time */
	time_t time_stamp;
};

static inline struct event *
create_event(int type)
{
	struct event *e;

	e = kmalloc(sizeof(struct event), GFP_KERNEL);
	memset(e, 0, sizeof(struct event));

	e->e_type = type;
	return e;
}

static inline void
destroy_event(struct event *event)
{
	kfree(event);
}

#endif	/* __EVENT_H__ */
