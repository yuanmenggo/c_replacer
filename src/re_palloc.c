#include "replacer.h"

re_pool_t *
re_create_pool(size_t size)
{
	re_pool_t	*p;
	p = re_memalign(RE_POOL_ALIGNMENT, size);
	if (p == NULL)
	{
		return NULL;
	}

	p->d.last = (u_char *) p + sizeof(re_pool_t);
	p->d.end  = (u_char *) p + size;
	p->d.next = NULL;
	p->d.failed = 0;
	p->current = p;
	p->chain = NULL;
	p->cleanup = NULL;

	return p;
}

void *
re_palloc(re_pool_t *pool, size_t size)
{
	u_char		*m;
	re_pool_t   *p;
	p = pool->current;
	do{
		m = re_align_ptr(p->d.last, RE_ALIGNMENT);
		if ((size_t)(p->d.end - m) >= size){
			p->d.last = m + size;
			return m;
		}
		p = p->d.next;
	} while (p);
	return re_palloc_block(pool, size);
}

void *
re_pnalloc(re_pool_t *pool, size_t size)
{
	u_char		*m;
	re_pool_t   *p;

	p = pool->current;
	do{
		m = p->d.last;
		if ((size_t)(p->d.end - m) >= size){
			p->d.last = m + size;
			return m;
		}
		p = p->d.next;
	} while(p);

	return re_palloc_block(pool, size);
}

static void *
re_palloc_block(re_pool_t *pool, size_t size)
{
	u_char		*m;
	size_t		psize;
	re_pool_t 	*p, *new, *current;

	psize = (size_t)(pool->d.end - (u_char *)pool);
	m = re_memalign(RE_POOL_ALIGNMENT, psize);
	if (m == NULL){
		return NULL;
	}

	new = (re_pool_t *) m;
	new->d.end = m + psize;
	new->d.next = NULL;
	new->d.failed = 0;

	m += sizeof(re_pool_data_t);
	m = re_align_ptr(m, RE_ALIGNMENT);
	new->d.last = m + size;

	current = pool->current;
	for (p = current; p->d.next; p = p->d.next){
		if (p->d.failed++ > 4){
			current = p->d.next;
		}
	}
	p->d.next = new;
	pool->current = current ? current : new;

	return m;
}

void *
re_memalign(size_t alignment, size_t size)
{
	void 	*p;
	int  	err;

	//p = memalign(alignment, size);
	if(size<= 0){
		size = 4096;
	}
	err	= posix_memalign(&p, alignment, size);
	if (err)
	{
		p = NULL;
	}
	return p;
}


void *
re_alloc(size_t size)
{
	void *p;
	p = malloc(size);
	if (p == NULL){
		exit_with_error("alloc memory error");
	}
	return p;
}

void *
re_pcalloc(re_pool_t *pool, size_t size)
{
	void *p;
	p = re_palloc(pool, size);
	if(p){
		re_memzero(p, size);
	}
	return p;
}

void
re_reset_pool(re_pool_t *pool)
{
	re_pool_t   		*p;
	for (p = pool; p; p = p->d.next){
		p->d.last = (u_char *)p + sizeof(re_pool_t);
		// re_memzero(p->d.last, p->d.end - p->d.last);
	}
}

void
re_destory_pool(re_pool_t *pool)
{
	re_pool_t   		*p, *n;
	for (p = pool, n = pool->d.next;; p=n, n=n->d.next){
		free(p);
		if (n == NULL){
			break;
		}
	}
}
