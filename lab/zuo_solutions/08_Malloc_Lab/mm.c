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

#include <iso646.h>

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

#define MAX(x, y) ((x) > (y)? (x) : (y))
#define MIN(x, y) ((x) < (y)? (x) : (y))
/*use first fit*/
#define FIRST_FIT

const void *null_void_ptr = (void *) -1;
const size_t alignment_mask = ALIGNMENT - 1;
/*
 * Extend heap by this amount (bytes) , default is 4 KB
 */
const size_t chunk_size = 1 << 12;

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
const size_t min_allocated_block_size = block_size_bytes;
const size_t min_free_block_size = ALIGN(block_size_bytes + ALIGNMENT);
const size_t mm_payloads_off = offsetof(mm_block_t, payloads);

/*
 * get block at address p
 * @param p the address of payloads, returned by mm_malloc or mm_realloc
 * @return block struct
 */
mm_block_t *get_mm_block(const void *p) {
    char *payloads = (char *) p;
    mm_block_t *block = (mm_block_t *) (payloads - mm_payloads_off);
    // check align and allocated tags,
    assert((block->block_size & alignment_mask) <= 3);
    return block;
}

/*
 * construct a block at block_start_p
 * @param block_start_p the address to construct a block
 * @param block_size the size of payloads
 * @param allocated_tags see description of allocated bits at mm_block_t
 * @return the constructed block
 */
mm_block_t *put_mm_block(void *block_start_p, size_t block_size, size_t allocated_tags) {
    // check align
    assert((block_size & alignment_mask) == 0);
    // check allocated tags
    assert(allocated_tags <= 3);
    mm_block_t *block = (mm_block_t *) block_start_p;
    block->block_size = block_size;
    // set allocated tag
    block->block_size |= allocated_tags;
    // if free ,set footer
    if (!(allocated_tags & allocated)) {
        size_t *footer_p = (size_t *) ((char *) block + block_size - block_size_bytes);
        *footer_p = block_size | allocated_tags;
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
    assert((block->block_size & alignment_mask) <= 3);
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
    assert(allocated_tags <= 3);
    return allocated_tags;
}

/*
 * set allocated tags
 * @param block the pointer of a block
 * @return old allocated tags
 */
size_t set_mm_allocated_tags(mm_block_t *block, size_t allocated_tags) {
    // check align and allocated tag
    assert(allocated_tags <= 3);
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
mm_block_t *next_mm_block(const mm_block_t *block) {
    size_t block_size = get_mm_block_size(block);
    if (is_mm_allocated(block)) {
        return (mm_block_t *) ((char *) block->payloads + block_size);
    } else {
        return (mm_block_t *) ((char *) block + block_size);
    }
}

/*
 * get prev block pointer, only for current block is free
 * @param p the address of payloads
 * @return the pointer of prev block
 */
mm_block_t *prev_mm_block(mm_block_t *block_p) {
    // check current block is free
    mm_block_t *prev_footer_p = (mm_block_t *) ((char *) block_p - block_size_bytes);
    size_t prev_block_size = get_mm_block_size(prev_footer_p);
    // get prev block address
    char *prev_b = (char *) block_p - prev_block_size;
    return (mm_block_t *) prev_b;
}

/*
 * Global variables
 */

// Pointer to first block
static char *heap_listp = 0; /*  */

/*
 * Function prototypes for internal helper routines
 */
static mm_block_t *extend_heap(size_t bytes);

static mm_block_t *find_fit(size_t adjusted_size);

static void place(mm_block_t *block, size_t adjusted_size);

static mm_block_t *coalesce(mm_block_t *block);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    assert(printf("assert should not print when release"));
    // the size of block_size must be less than ALIGNMENT, so that block can store block size info, no need more align space
    assert(block_size_bytes <= ALIGNMENT);
    // prologue block and epilogue block
    size_t init_size = 2 * min_free_block_size;
    size_t start_off = 0;
    /*
     * if block_size_bytes is less than align size, need padding
     *  in my machine, size_t(8) is equal to the align size, do not need padding
     */
    if (block_size_bytes < ALIGNMENT) {
        start_off = ALIGNMENT - block_size_bytes;
    }
    // Create the initial empty heap
    if ((heap_listp = mem_sbrk(init_size)) == null_void_ptr)
        return -1;
    // the prologue block and epilogue block are set allocated, for easy handle edge case
    mm_block_t *prologue_block = put_mm_block(heap_listp + start_off,
                                              ALIGNMENT,
                                              allocated);
    mm_block_t *epilogue_block = next_mm_block(prologue_block);
    // set the first block, not the prologue_block
    heap_listp = (char *) epilogue_block;
    // epilogue is allocated and size is zero, wasted ALIGNMENT bytes
    put_mm_block(epilogue_block, 0, prev_allocated | allocated);
    if (!extend_heap(chunk_size))
        return -1;
    return 0;
}

/**
 * extends the heap with a new free block
 * @param bytes the size of bytes need to be extended,
 * @return
 */
static mm_block_t *extend_heap(size_t bytes) {
    char *bp;
    // extend the size if needed, for maintain alignment
    size_t size = ALIGN(bytes);
    assert(size >= min_free_block_size);
    if ((bp = mem_sbrk(size)) == null_void_ptr)
        return NULL;
    mm_block_t *old_epilogue_block = (mm_block_t *) (bp - min_free_block_size);
    size_t new_tags = is_mm_prev_allocated(old_epilogue_block) | not_allocated;
    // Initialize old_epilogue_block to be free block
    mm_block_t *new_free_block = put_mm_block(old_epilogue_block,
                                              size,
                                              new_tags);
    mm_block_t *new_epilogue_block = next_mm_block(new_free_block);
    put_mm_block(new_epilogue_block, 0, allocated);

    // Coalesce if the previous block was free
    return coalesce(new_free_block);
}

/**
 *  coalesce free block
 * @param bp pointer of the payloads
 * @return
 */
static mm_block_t *coalesce(mm_block_t *block) {
    size_t curr_size = get_mm_block_size(block);
    // check free
    assert(!is_mm_allocated(block));
    size_t prev_alloc = is_mm_prev_allocated(block);
    mm_block_t *prev_block = prev_alloc ? NULL : prev_mm_block(block);
    // attention, pre allocate info in current block
    mm_block_t *next_block = next_mm_block(block);
    size_t next_alloc = is_mm_allocated(next_block);
    if (prev_alloc && next_alloc) {
        return block;
    } else if (prev_alloc && !next_alloc) {
        size_t next_size = get_mm_block_size(next_block);
        curr_size += next_size;
        put_mm_block(block, curr_size, prev_allocated | not_allocated);
    } else if (!prev_alloc && next_alloc) {
        size_t prev_size = get_mm_block_size(prev_block);
        curr_size += prev_size;
        // resset new block
        block = put_mm_block(prev_block, curr_size, prev_allocated | not_allocated);
    } else {
        size_t prev_size = get_mm_block_size(prev_block);
        size_t next_size = get_mm_block_size(next_block);
        curr_size += prev_size + next_size;
        // resset new block
        block = put_mm_block(prev_block, curr_size, prev_allocated | not_allocated);
    }
    return block;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    // ignore spurious request
    if (size == 0) {
        return NULL;
    }
    /**
     * Search the free list for a fit to put allocated block
     * so only to plus head tags, no need plus foot tags
     */
    size_t adjusted_size = ALIGN(block_size_bytes + size);
    mm_block_t *find_block = find_fit(adjusted_size);
    // No fit found. Get more memory and place the block
    if (!find_block) {
        size_t extend_size = MAX(adjusted_size, chunk_size);
        if (!((find_block = extend_heap(extend_size))))
            return NULL;
    }
    place(find_block, adjusted_size);
    return find_block->payloads;
}

/**
 * Find a fit for a block with adjusted size
 * @param adjusted_size
 * @return
 */
#ifdef FIRST_FIT
static mm_block_t *find_fit(size_t adjusted_size) {
    mm_block_t *head_block = (mm_block_t *) heap_listp;
    size_t curr_size;
    for (mm_block_t *current_block = head_block;
         (curr_size = get_mm_block_size(current_block)) > 0; // not the epilogue block
         current_block = next_mm_block(current_block)) {
        if ((!is_mm_allocated(current_block) && curr_size >= adjusted_size)) {
            return current_block;
        }
    }
    return NULL;
}
#else

#endif

/**
 * Place block of adjusted size at start of free block
 *   and split if remainder would be at least minimum block size
 * @param adjusted_size
 */
static void place(mm_block_t *block, size_t adjusted_size) {
    size_t block_size = get_mm_block_size(block);
    // the rest is enough to split free block
    size_t rest_size = block_size - adjusted_size;
    if (rest_size >= min_free_block_size) {
        // set allocated, need to subtract the header
        put_mm_block(block,
                     adjusted_size - block_size_bytes,
                     prev_allocated | allocated);
        block = next_mm_block(block);
        put_mm_block(block, rest_size, prev_allocated | not_allocated);
    } else {
        // the whole block, set allocated, need to subtract the header
        put_mm_block(block,
                     block_size - block_size_bytes,
                     prev_allocated | allocated);
        block = next_mm_block(block);
        set_mm_allocated_tags(block, prev_allocated | is_mm_allocated(block));
    }
}

/*
 * mm_free - Freeing a block
 */
void mm_free(void *ptr) {
    mm_block_t *mm_block = get_mm_block(ptr);
    /*free block size include header */
    put_mm_block(mm_block,
                 get_mm_block_size(mm_block) + block_size_bytes,
                 is_mm_prev_allocated(mm_block) | not_allocated);
    /*next block need to update pre allocated*/
    mm_block_t *next_block = next_mm_block(mm_block);
    set_mm_allocated_tags(next_block, get_mm_allocated_tags(next_block) & (~prev_allocated));
    coalesce(mm_block);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return mm_malloc(size);
    }
    if (!size) {
        mm_free(ptr);
        return NULL;
    }
    mm_block_t *curr_block = get_mm_block(ptr);
    // size_t adjusted_size = ALIGN(block_size_bytes + size);
    size_t curr_size = get_mm_block_size(curr_block);
    // size_t total_curr_size = block_size_bytes + curr_size;
    // if (adjusted_size <= curr_size) {
    //     // current block is allocated, need use padding size
    //     place(curr_block, adjusted_size);
    //     mm_block_t *next_block = next_mm_block(curr_block);
    //     if (!is_mm_allocated(next_block))
    //         coalesce(next_block);
    //     return ptr;
    // }
    // mm_block_t *next_block = next_mm_block(curr_block);
    // size_t next_size = get_mm_block_size(next_block);
    // if (!is_mm_allocated(next_block)) {
    //     mm_block_t *pre_block = prev_mm_block(curr_block);
    //     size_t prev_size = get_mm_block_size(pre_block);
    //     /* case 1 both pre and next block are free,
    //      * and it is sufficient to place new block
    //      * by merging pre,current and next block
    //      */
    //     size_t total_size = prev_size + total_curr_size + next_size;
    //     if (!is_mm_allocated(pre_block)) {
    //         if (total_size >= adjusted_size + block_size_bytes) {
    //             /* coalesce previous block, current block and next_block */
    //             put_mm_block(pre_block, total_size,
    //                 get_mm_allocated_tags(pre_block) & (~allocated));
    //             place(pre_block, adjusted_size);
    //             void *new_ptr = pre_block->payloads;
    //             /*attention, need to handle overlap case for need block and old block*/
    //             if (prev_size < curr_size) {
    //                 long overlap_i = curr_size - (prev_size + block_size_bytes);
    //                 memcpy(new_ptr, ptr, overlap_i );
    //                 memcpy(new_ptr + overlap_i, ptr + overlap_i, prev_size + block_size_bytes);
    //             } else {
    //                 memcpy(new_ptr, ptr, size);
    //             }
    //             return new_ptr;
    //         }
    //     }
    //     /* case 3 next block is free,
    //      * and it is sufficient to merge current block and next block */
    //     total_size = total_curr_size + next_size;
    //     if (total_size >= adjusted_size) {
    //         /* coalesce current block and next_block */
    //         put_mm_block(curr_block, total_size,
    //             get_mm_allocated_tags(curr_block) & (~allocated));
    //         /*current block is allocated, need use padding size*/
    //         place(curr_block, adjusted_size);
    //         return ptr;
    //     }
    // }
    /* case 2, add the neighbors of current block are not sufficient,
     * the above already free it, here find a new block
     */
    void *new_ptr = mm_malloc(size);
    size_t copy_size = MIN(size, curr_size);
    memcpy(new_ptr, ptr, copy_size);
    mm_free(ptr);
    return new_ptr;
}
