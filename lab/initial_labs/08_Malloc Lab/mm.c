/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "mm.h"
#include "memlib.h"

/********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 *******************************************************/
team_t team = {
    /* Team name */
    "anyun",
    /* First member's full name */
    "anyun zuo",
    /* First member's email address */
    "zsy296@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

const void *null_void_ptr = (void *)-1;
const size_t alignment_mask= ALIGNMENT-1;
/*
 * Extend heap by this amount (bytes) , default is 4 KB
 */
const size_t chunk_size = 1<<12;

const size_t allocated = 1;
const size_t not_allocated = 0;
const size_t prev_allocated = 2;


/*
 * heap block struct
 */
typedef struct {
    /*
     * the block size include the size of itself [sizeof(block_size) + sizeof(payloads)]
     * 2 lower bits are allocated bits, allocated bits can be 001, 000, 010
     *  the lowest bit for current
     *  the second-lowest bit for prev, only the lowest bit is 0, this bit is valid
     */
    size_t block_size;
    char payloads[0];
} mm_block_t;

const size_t block_size_bytes = sizeof(size_t);
const size_t mm_payloads_off = offsetof(mm_block_t, payloads);

/*
 * get block at address p
 * @param p the address of payloads, returned by mm_malloc or mm_realloc
 * @return block struct
 */
mm_block_t* get_mm_block(const void *p) {
    char *payloads = (char *) p;
    mm_block_t *block = (mm_block_t *) (payloads - mm_payloads_off);
    // check align and allocated tags,
    assert((block->block_size & alignment_mask) <= 2);
    return block;
}

/*
 * construct a block at block_start_p
 * @param block_start_p the address to construct a block
 * @param block_size the size of payloads
 * @param allocated_tags see description of allocated bits at mm_block_t
 * @return the constructed block
 */
mm_block_t* put_mm_block(void *block_start_p, size_t block_size, size_t allocated_tags) {
    // check align
    assert((block_size & alignment_mask) == 0);
    // check allocated tags
    assert(allocated_tags <= 2);
    mm_block_t *block = (mm_block_t *) block_start_p;
    block->block_size = block_size;
    // set allocated tag
    block->block_size |= allocated_tags;
    // if free ,set footer
    if (! (allocated_tags & allocated) ) {
        size_t *footer_p = (size_t *) (&(block->payloads[block_size]) - block_size_bytes);
        *footer_p = block_size;
    }
    return block;
}

/*
 * get the block size
 * @param block the pointer of a block
 * @return size fo the block
 */
size_t get_mm_block_size(const mm_block_t *block) {
    // check align and allocated tag
    assert((block->block_size & alignment_mask) <= 2);
    return block->block_size & ~alignment_mask;
}

/*
 * get allocated tags of the block
 * @param block the pointer of a block
 * @return allocated_tags
 */
size_t get_mm_allocated_tags(const mm_block_t *block) {
    size_t allocated_tags = block->block_size & alignment_mask;
    // check align and allocated tag
    assert(allocated_tags <= 2);
    return allocated_tags;
}

/*
 * set allocated tags
 * @param block the pointer of a block
 * @return old allocated tags
 */
size_t set_mm_allocated_tags(mm_block_t *block, size_t allocated_tags) {
    // check align and allocated tag
    assert(allocated_tags <= 2);
    size_t old_allocated_tags = get_mm_allocated_tags(block);
    size_t block_size = get_mm_block_size(block);
    put_mm_block(block, block_size, allocated_tags);
    return old_allocated_tags;
}

/*
 * check the block is allocated
 * @param block the pointer of a block
 * @return 1 if the block is allocated, 0 if free
 */
size_t is_mm_allocated(const mm_block_t *block) {
    return get_mm_allocated_tags(block) & allocated;
}

/*
 * check the prev block is allocated
 * @param block the pointer of a block
 * @return 1 if the prev block is allocated, 0 if free
 */
size_t is_mm_prev_allocated(const mm_block_t *block) {
    return get_mm_allocated_tags(block) & prev_allocated;
}

/*
 * set the block size
 * @param block the pointer of a block
 * @return old size
 */
size_t set_mm_block_size(mm_block_t *block, size_t new_size) {
    // check align
    assert((new_size & alignment_mask) == 0);
    size_t old_size = get_mm_block_size(block);
    size_t old_allocated_tags = get_mm_allocated_tags(block);
    put_mm_block(block, new_size, old_allocated_tags);
    return old_size;
}

/*
 * get next block pointer
 * @param block the pointer of current block
 * @return the pointer of next block
 */
mm_block_t* next_mm_block(const mm_block_t *block) {
    size_t block_size = get_mm_block_size(block);
    mm_block_t *next_p = (mm_block_t *) ((char*)block + block_size);
    return next_p;
}

/*
 * get prev block pointer, only for current block is free
 * @param p the address of payloads
 * @return the pointer of prev block
 */
mm_block_t* prev_mm_block(const void *p) {
    mm_block_t *block_p = get_mm_block(p);
    // check current block is free
    assert(!is_mm_allocated(block_p));
    mm_block_t *prev_footer_p = (mm_block_t *) ((char*)block_p - block_size_bytes);
    size_t prev_block_size = get_mm_block_size(prev_footer_p);
    // get prev block address
    char* prev_b = (char*)block_p - prev_block_size;
    return (mm_block_t*) prev_b;
}

/*
 * Global variables
 */

// Pointer to first block
static char *heap_listp = 0;  /*  */

/*
 * Function prototypes for internal helper routines
 */
static void *extend_heap(size_t bytes);
static mm_block_t* find_fit(size_t adjusted_size);
static void place(mm_block_t* block, size_t adjusted_size);
static void *coalesce(void *bp);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    assert(printf("assert should not print when release"));
    // the size of block_size must be less than ALIGNMENT, so that block can store block size info, no need more align space
    assert(block_size_bytes <= ALIGNMENT);
    // prologue block and epilogue block
    size_t init_size = 2 * block_size_bytes;
    size_t start_off = 0;
    /*
     * if block_size_bytes is less than align size, need padding
     *  in my machine, size_t(8) is equal to the align size, do not need padding
     */
    if (block_size_bytes < ALIGNMENT) {
        init_size = 2 * ALIGNMENT;
        start_off= ALIGNMENT - block_size_bytes;
    }
    // Create the initial empty heap
    if ((heap_listp = mem_sbrk(init_size)) == null_void_ptr)
        return -1;
    heap_listp += start_off;
    // the prologue block and epilogue block are set allocated, for easy handle edge case
    mm_block_t* prologue_block = put_mm_block(heap_listp, ALIGNMENT, allocated);
    mm_block_t* epilogue_block = next_mm_block(prologue_block);
    put_mm_block(epilogue_block, ALIGNMENT, allocated);
    if(extend_heap(chunk_size) == NULL)
        return -1;
    return 0;
}

/**
 * extends the heap with a new free block
 * @param bytes the size of bytes need to be extended,
 * @return
 */
static void *extend_heap(size_t bytes)
{
    char *bp;
    // extend the size if needed, for maintain alignment
    size_t size = ALIGN(bytes);
    if ((bp = mem_sbrk(size)) == null_void_ptr)
        return NULL;
    mm_block_t* old_epilogue_block = get_mm_block(bp);

    // Initialize old_epilogue_block to be free block
    mm_block_t* new_free_block = put_mm_block(old_epilogue_block, size, not_allocated);
    mm_block_t* new_epilogue_block = next_mm_block(new_free_block);
    put_mm_block(new_epilogue_block, ALIGNMENT, allocated);

    // Coalesce if the previous block was free
    return coalesce(new_free_block);
}

/**
 *  coalesce free block
 * @param bp pointer of the payloads
 * @return
 */
static void *coalesce(void *bp) {
    mm_block_t* curr_block = get_mm_block(bp);
    size_t curr_size = get_mm_block_size(curr_block);
    // check free
    assert(!is_mm_allocated(curr_block));
    mm_block_t *prev_block = prev_mm_block(curr_block);
    size_t prev_alloc = is_mm_allocated(prev_block);
    mm_block_t *next_block = next_mm_block(curr_block);
    size_t next_alloc = is_mm_allocated(next_block);
    if (prev_alloc && next_alloc) {
        return bp;
    } else if (prev_alloc && !next_alloc) {
        size_t next_size = get_mm_block_size(next_block);
        curr_size += next_size;
        put_mm_block(curr_block, curr_size, not_allocated);
    } else if (!prev_alloc && next_alloc) {
        size_t prev_size = get_mm_block_size(prev_block);
        curr_size += prev_size;
        // resset new block
        curr_block = put_mm_block(prev_block, curr_size, not_allocated);
    } else {
        size_t prev_size = get_mm_block_size(prev_block);
        size_t next_size = get_mm_block_size(next_block);
        curr_size += prev_size + next_size ;
        // resset new block
        curr_block = put_mm_block(prev_block, curr_size, not_allocated);
    }
    return curr_block->payloads;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t adjusted_size; // adjusted block size
    size_t extend_size; // amount to extend heap if not fit
    mm_block_t* find_block;
    // ignore spurious request
    if (size == 0) {
        return NULL;
    }
    adjusted_size = ALIGN(size + block_size_bytes);
    find_block = find_fit(adjusted_size);
    if (find_block) {
        place(find_block, adjusted_size);
        return find_block->payloads;
    }
    void *p = mem_sbrk(newsize);
    if (p == (void *) -1)
        return NULL;
    else {
        *(size_t *) p = size;
        return (void *) ((char *) p + block_size_bytes);
    }
}

/**
 * Find a fit for a block with adjusted size
 * @param adjusted_size
 * @return
 */
static mm_block_t* find_fit(size_t adjusted_size) {

}

/**
 * Place block of adjusted size at start of free block
 *   and split if remainder would be at least minimum block size
 * @param adjusted_size
 */
static void place(mm_block_t* block, size_t adjusted_size) {

}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *) ((char *) oldptr - block_size_bytes);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
