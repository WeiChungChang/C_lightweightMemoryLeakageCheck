#ifndef _mmdbg_h_
#define _mmdbg_h_

#include <sys/queue.h>

#define DBG_MEM_LEAK 1

#ifndef DBG_MEM_LEAK
#define free_dbg(p) free((p))
#define malloc_dbg(p) malloc((p))
#else
#define  free_dbg(p) ({printf("[%s:%s():%d] free ptr (%p)\n", __FILE__, __FUNCTION__, __LINE__, (p)); _free_dbg(p);})
#define  malloc_dbg(sz) ({void *ret = _malloc_dbg((sz)); if (ret){ printf("[%s:%s():%d] malloc sz = %lu ptr = %p\n", __FILE__, __FUNCTION__, __LINE__, (sz), ret);} ret;})
#endif

TAILQ_HEAD(tailhead, entry) head;

struct entry {
	void   *p;
	size_t size;
	TAILQ_ENTRY(entry) entries; 	/* Tail queue. */
} *np;

int memory_dbg_init();

int memory_dbg_finalize();

void* _malloc_dbg(size_t size);

void _free_dbg(void* p);

#endif
