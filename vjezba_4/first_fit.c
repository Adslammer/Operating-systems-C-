/*!  Dynamic memory allocator - first fit (relocatable) */

#define _FIRST_FIT_C_
#include "first_fit.h"

static ff_internal_t *ffi = NULL; /* pointer for ff internal structure */

/*!
 * Initialize dynamic memory manager
 * \param start Memory pool start address
 * \param size Memory pool size
 * \param pshared Is allocator used by more processes simultaneously?
 *		  (Required for lock initialization)
 */
void ff_init ( void *start, unsigned int size, int pshared )
{
	int shift;
	header_t *block;

	if ( ffi )
		return; /* already initialized */

	/* align 'start' to 'int' boundary */
	shift = ( (int) start ) & ( sizeof (int) - 1 );
	if ( shift > 0 )
	{
		shift = sizeof (int) - shift;
		start += shift;
		size -= shift;
	}

	ffi = start; /* internal data is also placed in memory pool */

	if ( ffi->init == FF_MAGIC )
		return; /* there is an valid ff data structure, use it */


	LOCK_INIT ( pshared );
	LOCK ();

	start += sizeof (ff_internal_t);
	size -= sizeof (ff_internal_t);

	/* align size to lower 'int' boundary */
	size &= ~( sizeof (int) - 1 );

	if ( size < 2 * HEADER_SIZE )
	{
		printf ( "Pool size too small!\n" );

		UNLOCK ();
		ffi = NULL;

		return;
	}

	ffi->fl = OFFSET(start);
	block = HEADER(ffi->fl);
	block->size = size;
	block->pf = block->nf = block->pa = block->na = NULL;

	ffi->init = FF_MAGIC;

	UNLOCK ();
}

/*!
 * Destroy memory allocator
 * Currently, since memory pool is given, nothing is realy 'destroyed'. Internal
 * data structure is just reset.
 */
void ff_destroy ()
{
	if ( ffi )
	{
		ffi->init = 0;
		ffi = NULL;	/* just mark as uninitilized */
	}
}

/*!
 * Get free block with required size (or slightly bigger)
 * \param size Requested block size
 * \returns Block address, NULL if can't find adequate free block
 */
void *ff_alloc ( unsigned int size )
{
	header_t *iter, *block;

	LOCK ();

	size += HEADER_SIZE; /* add header size to requested size */

	/* align request size to higher 'int' boundary */
	size = ( size + sizeof (int) - 1 ) & ~( sizeof (int) - 1 );

	iter = ffi->fl;
	while ( iter != NULL && HEADER(iter)->size < size )
		iter = HEADER(iter)->nf;

	if ( iter == NULL )
	{
		UNLOCK ();
		return NULL; /* no adequate free block found */
	}

	iter = HEADER(iter);
	if ( iter->size >= size + 2 * HEADER_SIZE )
	{
		/* split block */
		/* first part remains in free list, just update size */
		iter->size -= size;
		block = ( (void *) iter ) + iter->size;
		block->size = size;

		block->pa = OFFSET(iter);
		block->na = iter->na;
		iter->na = OFFSET(block);
		if ( block->na != NULL )
			HEADER(block->na)->pa = OFFSET(block);
	}
	else { /* give whole block */
		block = iter;

		/* remove it from free list */
		ff_remove_block ( OFFSET(block) );
	}

	MARK_USED ( block ); /* mark it as used */

	UNLOCK ();

	return (void *) (block + 1); /* first usable address after header */
}

/*!
 * Free memory block
 * \param block Block location (starting address)
 * \returns 0 if successful, -1 otherwise
 */
int ff_free ( void *block )
{
	header_t *b;
	int joined;

	LOCK ();

	b = block;
	b--;/* return to block header */
	MARK_FREE ( b );
	joined = 0;

	/* join with left? */
	if ( b->pa != NULL && CHECK_FREE ( HEADER(b->pa) ) )
	{
		joined = 1;
		HEADER(b->pa)->size += b->size;

		HEADER(b->pa)->na = b->na;

		if ( b->na != NULL )
			HEADER(b->na)->pa = b->pa;

		b = HEADER(b->pa);
	}

	/* join with right? */
	if ( b->na != NULL && CHECK_FREE ( HEADER(b->na) ) )
	{
		if ( joined ) /* 'b' in free list? (joined with left) */
		{
			/* join 'right' block with 'b' */

			b->size += HEADER(b->na)->size;

			/* remove 'b->na' from free list */
			ff_remove_block ( b->na );
			b->na = HEADER(b->na)->na;
			if ( b->na != NULL )
				HEADER(b->na)->pa = OFFSET(b);
		}
		else {
			/* join 'b' with right block */
			joined = 1;

			b->size += HEADER(b->na)->size;

			b->pf = HEADER(b->na)->pf;
			b->nf = HEADER(b->na)->nf;

			if ( ffi->fl == b->na )
				ffi->fl = OFFSET(b);

			if ( b->pf != NULL )
				HEADER(b->pf)->nf = OFFSET(b);

			if ( b->nf != NULL )
				HEADER(b->nf)->pf = OFFSET(b);

			b->na = HEADER(b->na)->na;
			if ( b->na != NULL )
				HEADER(b->na)->pa = OFFSET(b);
		}
	}

	if ( !joined ) /* not joined with left nor right? */
	{
		/* put it at start of the free list */
		b->pf = NULL;
		b->nf = ffi->fl;
		ffi->fl = OFFSET(b);
		if ( b->nf != NULL )
			HEADER(b->nf)->pf = OFFSET(b);
	}

	UNLOCK ();

	return 0;
}

/*!
 * Routine that removes an block from 'free' list (ffi->fl)
 * \param b block address (relative to 'ffi')
 */
static inline void ff_remove_block ( header_t *b )
{
	header_t *hb;

	hb = HEADER(b);

	/* remove it from free list */
	if ( b == ffi->fl )
		ffi->fl = hb->nf;
	else
		HEADER(hb->pf)->nf = hb->nf;
		/* 'b' has previous free block since its not first */

	if ( hb->nf != NULL )
		HEADER(hb->nf)->pf = hb->pf;
}
