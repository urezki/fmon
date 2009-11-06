#ifndef __EVENT_H__
#define __EVENT_H__

#define E_CRE 0x00000001		/* create  */
#define E_WRI 0x00000002		/* write   */
#define E_SYM 0x00000004		/* symlink */
#define E_LIN 0x00000008		/* link    */
#define E_UNL 0x00000010		/* unlink  */
#define E_MKD 0x00000020		/* mkdir   */
#define E_RMD 0x00000040		/* rmdir   */
#define E_MKN 0x00000100		/* mknod   */
#define E_REN 0x00000200		/* rename  */

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
