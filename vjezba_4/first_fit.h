/*! Dynamic memory allocator - first fit (relocatable)
 *
 * In this implementation double linked list are used.
 * Free list is not sorted. Search is started from first element until block
 * with adequate size is found (same or greater than required).
 * When block is freed, first join is tried with left and right neighbor block
 * (by address). If not joined, block is marked as free and put at list start.
 *
 * Code is adjusted to be independent of starting address (relocatable). This is
 * required if more programs (processes) uses same memory pool, but which has
 * different starting addresses in different programs (mapped to different
 * place).
 *
 * (prilagodjeno za 4. laboratorijsku vjezbu iz OSa)
 */

#pragma once

/* interface */

void ff_init ( void *start, unsigned int size, int pshared );
void ff_destroy ();
void *ff_alloc ( unsigned int size );
int ff_free ( void *block );


#ifdef _FIRST_FIT_C_

/* block header (for both used and free blocks) */
typedef struct _header_t_
{
	unsigned int size; /* block size, including head and tail headers */
	void *pf; /* previous free in list - relative to segment start (ffi) */
	void *nf; /* next free in list (relative) */
	void *pa; /* previous by address (relative) */
	void *na; /* next by address (relative) */
}
header_t;

/* use LSB of P to mark it as free (otherwise size is always even) */
#define MARK_USED(P)	(P)->size |= 1
#define MARK_FREE(P)	(P)->size &= ~1U
#define CHECK_USED(P)	((P)->size & 1)
#define CHECK_FREE(P)	!CHECK_USED(P)

#define HEADER_SIZE	( sizeof (header_t) )

typedef unsigned long int aint; /* integer that is equal in size as pointer */
#define AADD(a,b)       ( (void *) ( ( (aint) (a) ) + ( (aint) (b) ) ) )
#define ASUB(a,b)       ( (void *) ( ( (aint) (a) ) - ( (aint) (b) ) ) )

#define OFFSET(adr)	ASUB(adr,ffi)
#define HEADER(adr)	( (header_t *) AADD(adr,ffi) )


/*! >>> external dependency */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ERROR(format, ...)	printf(format, ##__VA_ARGS__)

/* synchronization */
#define LOCK()			pthread_mutex_lock ( &ffi->key )
#define UNLOCK()		pthread_mutex_unlock ( &ffi->key )
#define LOCK_VARIABLES		pthread_mutex_t key;
#define LOCK_INIT(shared)						\
{									\
	pthread_mutexattr_t attr;					\
									\
	if ( shared )							\
	{								\
		pthread_mutexattr_init ( &attr );			\
		pthread_mutexattr_setpshared ( &attr, PTHREAD_PROCESS_SHARED );\
		pthread_mutex_init ( &ffi->key, &attr );		\
	}								\
	else {								\
		pthread_mutex_init ( &ffi->key, NULL );			\
	}								\
}

/* constant NULL */
#ifndef NULL
#define NULL	( (void *) 0 )
#endif

/*! <<< external dependency */

/* internal data structure */
typedef struct _ff_internal_
{
	header_t *fl;		/* list of free blocks (free list) */
	LOCK_VARIABLES		/* key for locking (if used) */
	unsigned int init; 	/* is data already initilized (and running)? */
}
ff_internal_t;

#define FF_MAGIC	0xaaaaaaaa	/* initialization constant */

static inline void ff_remove_block ( header_t *b );

#endif /* _FIRST_FIT_C_ */
