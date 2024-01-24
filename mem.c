#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "mem.h"

#define PAGESIZE (size_t) getpagesize()
#define MEM_ALIGN(size) ((size + 7) & ~7) // ensure allocation size is 8-byte aligned

// represent a free memory block
typedef struct Block
{
    size_t size;        // size of a memory block
    struct Block *next; // points to the next free block if it's in the free list.
    int magic;          // Used to determine at release time whether the current block of memory is the one allocated by a previous call to mem_alloc
} Block;

static void *memory = NULL;           // pointer to the allocated memory region
static size_t memory_size = 0;        // size of the allocated memory region
static Block *free_list = NULL;       // Linked list of free memory blocks
static size_t max_size = 0;           // point to the max block from the start to curr
static Block *search_prt = NULL;      // point to the history addr of FT
static Block *prev_search_prt = NULL; // point to the prev block of search_prt
int m_error = 0;

int Mem_Init(int size_of_region)
{
    if (memory != NULL || size_of_region <= 0)
    { // region!=NULL indicates that Mem_Init has been called previously,size_of_region<=0 is an illegal memory request
        m_error = E_BAD_ARGS;
        return -1;
    }

    // ensure size_of_region is a multiple of the pagesize
    size_of_region = ((size_of_region + PAGESIZE - 1) / PAGESIZE) * PAGESIZE;

    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    memory = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memory == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    close(fd); // close the device (don't worry, mapping should be unaffected)

    memory_size = size_of_region;

    free_list = (Block *)memory;
    free_list->size = size_of_region - sizeof(Block);
    free_list->next = NULL;

    return 0;
}
void Split_Block(Block *free_block, Block *prev_free_block, int size)
{
    // split the block
    if (free_block->size > size + sizeof(Block))
    {
        Block *new_free_block = (Block *)((char *)free_block + size + sizeof(Block));
        new_free_block->size = free_block->size - size - sizeof(Block);
        new_free_block->next = free_block->next;
        free_block->size = size;
        free_block->next = NULL;
        if (prev_free_block == NULL)
        {
            free_list = new_free_block;
            prev_search_prt = NULL;
        }
        else
        {
            prev_free_block->next = new_free_block;
            prev_search_prt = prev_free_block;
        }
        search_prt = new_free_block;
    }
    else
    {
        if (prev_free_block == NULL)
        {
            free_list = free_block->next;
            prev_search_prt = NULL;
        }
        else
        {
            prev_free_block->next = free_block->next;
            prev_search_prt = prev_free_block;
        }
        search_prt = free_block->next;
    }
}
void *Mem_Alloc_Origi(int size, int style)
{
    //printf("---Alloc---\n");
    if (size <= 0 || (style != M_BESTFIT && style != M_WORSTFIT && style != M_FIRSTFIT))
    {
        m_error = E_BAD_ARGS;
        return NULL;
    }
    size = MEM_ALIGN(size);
    Block *prev = NULL;
    Block *curr = free_list;
    Block *prev_free_block = NULL; // points to the previous free block of the best free block found
    Block *free_block = NULL;                 // points to the best free block found
    int free_block_size;
    if (style == M_BESTFIT)
    {
        free_block_size = memory_size;
    }
    else if (style == M_WORSTFIT)
    {
        free_block_size = 0;
    }

    while (curr != NULL)
    {
        //printf("curr1_size=%d\n", (int)curr->size);
        if (curr->size >= size)
        {
            if (style == M_FIRSTFIT)
            {
                prev_free_block = prev;
                free_block = curr;
                break;
            }
            else if (style == M_BESTFIT && curr->size < free_block_size)
            {
                prev_free_block = prev;
                free_block = curr;
                free_block_size = curr->size;
            }
            else if (style == M_WORSTFIT && curr->size > free_block_size)
            {
                prev_free_block = prev;
                free_block = curr;
                free_block_size = curr->size;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    if (free_block == NULL)
    {
        m_error = E_NO_SPACE;
        return NULL;
    }
    Split_Block(free_block, prev_free_block, size);

    free_block->magic = 123456;

    return (void *)(free_block + 1);
}

void *Mem_Alloc(int size, int style)
{
    //printf("---Alloc---\n");
    if (size <= 0 || (style != M_BESTFIT && style != M_WORSTFIT && style != M_FIRSTFIT))
    {
        m_error = E_BAD_ARGS;
        return NULL;
    }

    size = MEM_ALIGN(size);
    Block *prev = NULL;
    Block *curr = free_list;
    Block *prev_free_block = NULL; // points to the previous free block of the best free block found
    Block *free_block = NULL;                 // points to the best free block found
    int free_block_size;
    if (style == M_BESTFIT)
    {
        free_block_size = memory_size;
    }
    else if (style == M_WORSTFIT)
    {
        free_block_size = 0;
    }
/*     printf("free_list=%d,",(int)free_list->size);
    printf("size=%d,",(int)size);
    printf("max_size=%d,",(int)max_size); */
    if (search_prt != NULL && max_size != 0 && size > max_size)
    {
        //printf("search_prtt=%d,",(int)search_prt->size);
        prev = prev_search_prt;
        curr = search_prt;
    }
    //printf("\n");

    while (curr != NULL)
    {
        //printf("curr_size=%d\n", (int)curr->size);
        if (curr->size >= size)
        {
            if (style == M_FIRSTFIT)
            {
                prev_free_block = prev;
                free_block = curr;
                break;
            }
            else if (style == M_BESTFIT && curr->size < free_block_size)
            {
                prev_free_block = prev;
                free_block = curr;
                free_block_size = curr->size;
            }
            else if (style == M_WORSTFIT && curr->size > free_block_size)
            {
                prev_free_block = prev;
                free_block = curr;
                free_block_size = curr->size;
            }
        }
        max_size = max_size < curr->size ? curr->size : max_size;
        prev = curr;
        curr = curr->next;
    }

    if (free_block == NULL)
    {
        m_error = E_NO_SPACE;
        return NULL;
    }
    Split_Block(free_block, prev_free_block, size);

    free_block->magic = 123456;

    return (void *)(free_block + 1);
}

int Mem_Free(void *ptr)
{
    search_prt = NULL;
    prev_search_prt = NULL;
    if (ptr == NULL)
    {
        return 0;
    }

    Block *block = (Block *)((char *)ptr - sizeof(Block));
    if (block < (Block *)memory || block >= (Block *)((char *)memory + memory_size) || block->magic != 123456)
    {
        m_error = E_BAD_POINTER;
        return -1;
    }

    block->magic = 0;

    if (free_list == NULL)
    {
        free_list = block;
    }
    else
    {
        Block *prev = NULL;
        Block *curr = free_list;
        while (curr != NULL && (char *)curr < (char *)block)
        {
            prev = curr;
            curr = curr->next;
        }
        if (((char *)block + block->size + sizeof(Block)) == (char *)curr)
        {
            block->size += curr->size + sizeof(Block);
            block->next = curr->next;
        }
        else
        {
            block->next = curr;
        }
        if (prev != NULL)
        {
            if (((char *)prev + prev->size + sizeof(Block)) == (char *)block)
            {
                prev->size += block->size + sizeof(Block);
                prev->next = block->next;
            }
            else
            {
                prev->next = block;
            }
        }
        else
        {
            free_list = block;
        }
    }
    return 0;
}
void Mem_Dump()
{
    Block *curr = free_list;
    while (curr != NULL)
    {
        printf("Free Block: address=%p,size=%zu\n", curr, curr->size);
        curr = curr->next;
    }
}