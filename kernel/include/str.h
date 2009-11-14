#ifndef __STR_H__
#define __STR_H__

extern struct qstr *make_qstr(const char *, int, int);
extern void free_qstr(struct qstr *);

#endif	/* __STR_H__ */
