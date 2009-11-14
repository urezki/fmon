#include <linux/fs.h>
#include <linux/string.h>
#include <linux/dcache.h>

/* locals */
#include <str.h>

struct qstr *
make_qstr(const char *name, int len, int hash)
{
	struct qstr *q;
	char *c;

	q = kmalloc(sizeof(*q), GFP_KERNEL);
	c = kmalloc(sizeof(char) * len, GFP_KERNEL);
	memcpy(c, name, len);
	q->name = c;
	q->len = len;
	q->hash = hash;

	return q;
}

void
free_qstr(struct qstr *q)
{
	if (q && q->name)
		kfree(q->name);

	if (q)
		kfree(q);
}

