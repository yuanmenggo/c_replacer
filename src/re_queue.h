#include "replacer.h"


#ifndef _RE_QUEUE_H_INCLUDED_
#define _RE_QUEUE_H_INCLUDED_

typedef struct re_queue_s  	re_queue_t;

struct re_queue_s{
	re_queue_t	*prev;
	re_queue_t	*next;
	void		*data;
};

#define re_queue_init(q)													\
		(q)->prev = q;														\
		(q)->next = q

#define	re_queue_empty(h)													\
		(h == (h)->prev)


#define	re_queue_insert_head(h, x)											\
		(x)->next = (h)->next;												\
		(x)->next->prev = x;												\
		(x)->prev = h;														\
		(h)->next = x

#define  re_queue_insert_after	re_queue_insert_head

#define	 re_queue_insert_tail(h, x) 										\
		 (x)->prev = (h)->prev;												\
		 (x)->prev->next = x;												\
		 (x)->next = h;														\
		 (h)->prev = x	

#define re_queue_head(h)													\
		 (h)->next

#define re_queue_last(h)													\
		 (h)->prev

#define re_queue_next(q)													\
		 (q)->next

#define re_queue_prev(q)													\
		 (q)->prev

#define re_queue_remove(x)													\
		 (x)->next->pre = (x)->prev;										\
		 (x)->prev->next = (x)->next


#define re_queue_split(h, q, n)												\
    	(n)->prev = (h)->prev;                                              \
    	(n)->prev->next = n;                                                \
    	(n)->next = q;                                                      \
    	(h)->prev = (q)->prev;                                              \
    	(h)->prev->next = h;                                                \
    	(q)->prev = n;		

#define re_queue_add(h, n)                                                   \
    	(h)->prev->next = (n)->next;                                         \
    	(n)->next->prev = (h)->prev;                                         \
    	(h)->prev = (n)->prev;                                               \
    	(h)->prev->next = h; 


#endif /* _RE_QUEUE_H_INCLUDED_ */