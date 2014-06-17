#include "replacer.h"

#ifndef _RE_PALLOC_H_INCLUDED_
#define _RE_PALLOC_H_INCLUDED_

typedef struct re_pool_s  			re_pool_t;
typedef struct re_pool_data_s  		re_pool_data_t;
typedef struct re_pool_cleanup_s  	re_pool_cleanup_t;
typedef struct re_chain_s  			re_chain_t;
typedef struct re_buf_s   			re_buf_t;
typedef struct re_log_s   			re_log_t;

typedef void (*re_pool_cleanup_pt)(void *data);

struct re_log_s
{
	void			 *data;
};

struct re_pool_data_s
{
	u_char		*last;
	u_char		*end;
	re_pool_t   *next;
	uintptr_t	failed;
};

struct re_pool_s
{
	re_pool_data_t		d;
	size_t				max;
	re_pool_t   		*current;
	re_chain_t			*chain;
	re_pool_cleanup_t	*cleanup;
	re_log_t			*log;
};


struct re_pool_cleanup_s
{
	re_pool_cleanup_pt handler;
	void			   *data;
	re_pool_cleanup_t  *next;
};

struct re_chain_s
{
	re_buf_t	*buf;
	re_chain_t	*next;
};

struct re_buf_s{
        u_char  *pos;
        u_char  *last;
        u_char  *start;
        u_char  *end;
};

#define re_align(d, a)		(((d) + (a - 1)) & ~(a - 1))
#define	re_align_ptr(p, a)												 \
	(u_char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t) a - 1))

#define	re_memzero(buf, n)	 (void) memset(buf, 0, n)
#define re_memset(buf, c, n) (void)	memset(buf, c, n)

static void * re_palloc_block(re_pool_t *, size_t);
void * re_memalign(size_t, size_t);
re_pool_t * re_create_pool(size_t);
void * re_pnalloc(re_pool_t *, size_t);
void * re_palloc(re_pool_t *, size_t);
void * re_pcalloc(re_pool_t *, size_t);
void * re_alloc(size_t);
void re_reset_pool(re_pool_t *);
void re_destory_pool(re_pool_t *);

#endif /* _RE_PALLOC_H_INCLUDED_ */