#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mmdbg.h"

static size_t total_alloc = 0;
static size_t total_free  = 0;

static pthread_mutex_t mutex;

int memory_dbg_init()
{
	TAILQ_INIT(&head);
	return pthread_mutex_init(&mutex, NULL);
}

void* _malloc_dbg(size_t size)
{
	void *ret = malloc(size);
	if (ret) {
		np = malloc(sizeof(struct entry));		/* Insert at the tail. */
		np->p    = ret;
		np->size = size;
	} else {
		return NULL;
	}

	/*critical region*/
	pthread_mutex_lock(&mutex);
	total_alloc += size;
	TAILQ_INSERT_TAIL(&head, np, entries);
	pthread_mutex_unlock(&mutex);

	return ret;
}

void _free_dbg(void* p)
{
	/*critical region*/
	pthread_mutex_lock(&mutex);
	TAILQ_FOREACH(np, &head, entries)
	{
		if (np->p == p) {
			total_free += np->size;
			TAILQ_REMOVE(&head, np, entries);
			free(np);
			free(p);
			pthread_mutex_unlock(&mutex);
			return;
		}
	}
	pthread_mutex_unlock(&mutex);
}

int memory_dbg_finalize()
{
	pthread_mutex_lock(&mutex);
	np	= TAILQ_FIRST(&head);
	while (np!= NULL) {
		printf("[%s:%s():%d] potential memory leakage!!! np %p (ptr = %p, size = %lu)\n", __FILE__, __FUNCTION__, __LINE__, np, np->p, np->size);
		TAILQ_REMOVE(&head, np, entries);
		free(np->p);
		np = TAILQ_NEXT(np, entries);
	}
	pthread_mutex_unlock(&mutex);
	return pthread_mutex_destroy(&mutex);
}

static int _tailq_size()
{
	pthread_mutex_lock(&mutex);
	int num = 0;
	TAILQ_FOREACH(n1, &head, entries)
	{
		num++;
	}
	pthread_mutex_unlock(&mutex);
	return num;
}
