#include "malloc_perso.h"
#include "free_perso.h"
#include <unistd.h>
#include <stdio.h>

#define ALIGNED_SIZE 16

void* g_head = NULL;
void* g_free_list = NULL;


void initialize_block(t_block* block)
{
    if (block == NULL)
        return;

    block->size = 0;
    block->next = NULL;
    block->previous = NULL;
    block->free = false;
}

void insert_block(t_block* block, void** list)
{
    if (block == NULL)
        return;

    if (*list == NULL)
        *list = block;
    else
    {
        t_block* iterator = *list;
        while(iterator->next != NULL)
            iterator = iterator->next;

        iterator->next = block;
        block->previous = iterator;
    }
}

t_block* expend_heap(size_t size)
{
    printf("[MALLOC INTERNAL] I'm going to alloc %ld bytes (sizeof t_block: %ld + requested size %ld (alignment %d))\n",
        size + sizeof(t_block), sizeof(t_block), size, ALIGNED_SIZE);

    t_block* block = (t_block*)sbrk(0);

    if (sbrk(sizeof(t_block) + size) == (void*)-1)
        return NULL;

    initialize_block(block);
    block->size = size;

    insert_block(block, &g_head);

    printf("[MALLOC INTERNAL] The t_block pointer is %p / sizeof(t_block) = %ld / so data block begins at %p\n",
        (void*)block, sizeof(t_block), (void*)(block + 1));
    return block;
}

void* malloc_perso(size_t size)
{
    if (size == 0)
        return NULL;

    pad_size(&size);

    t_block* block = find_block(size);
    if (block == NULL)
    {
        block = expend_heap(size);
        printf( "[MALLOC INTERNAL] I found no free block: created a new one %p\n", (void*)block);
    }

    if (block == NULL)
        return NULL;
    
    return ++block;
}

void pad_size(size_t* size)
{
    if (size == NULL)
        return;

    unsigned int diff = *size % ALIGNED_SIZE;
    if (diff == 0)
        return;

    *size += ALIGNED_SIZE - diff;
}

bool is_valid_adress(t_block* block)
{
    if (block == NULL)
        return false;

    if ( !((void*)block >= g_head && (void*)block < sbrk(0)) )
        return false;

    t_block* iterator = g_head;
    while(iterator != NULL)
    {
        if (iterator == block)
            return true;

        iterator = iterator->next;
    }

    return false;
}

t_block* find_block(size_t size)
{
    t_block* iterator = g_head;
    while(iterator != NULL)
    {
        if (iterator->free && iterator->size >= size)
        {
            printf("[MALLOC INTERNAL] I found free block %p, I'm going to return %p\n", (void*)iterator, (void*)(iterator + 1) );

            if (iterator->size > size)
                split_block(iterator, size);

            iterator->free = false;
            return iterator;
        }

        iterator = iterator->next;
    }

    return NULL;
}

bool split_block(t_block* b, size_t size)
{
    if (b == NULL)
        return false;

    if (b->size < sizeof(t_block) + size)
        return false;

    printf("[MALLOC INTERNAL] Block %p of size %ld can be split when allocating %ld bytes because %ld > minimum Split Block Size %ld\n",
        (void*)b, b->size, size, b->size, sizeof(t_block) + size);

    t_block* block = (t_block*)(((char*)b) + (sizeof(t_block) + size));
    block->size = b->size - (sizeof(t_block) + size);
    block->free = true;
    block->next = b->next;
    block->previous = b;

    printf("[MALLOC INTERNAL] Splitting Block %p of size %ld : Resizing it to size %ld and creating a new block %p having size %ld.\n",
        (void*)b, b->size, size + sizeof(t_block), (void*)block, block->size);

    b->size = size;
    if (b->next != NULL)
        b->next->previous = block;
    b->next = block;

    return true;
}

bool try_to_fusion(t_block* b)
{
    if (b == NULL)
        return false;

    bool succeed = false;
    
    succeed = try_to_fusion_previous(b)? true: succeed;
    succeed = try_to_fusion_next(b)? true: succeed;

    return succeed;
}

bool try_to_fusion_next(t_block* b)
{
    if (b == NULL)
        return false;

    if ((b->next != NULL) && b->next->free)
    {
        printf("[MALLOC INTERNAL] Merging block %p with next one %p because both are free\n", (void*)b, (void*)b->next);
        b->size += b->next->size + sizeof(t_block);
        if (b->next->next != NULL)
            b->next->next->previous = b;
        b->next = b->next->next;
        printf("[MALLOC INTERNAL] Block %p is now of size %ld.\n", (void*)b, b->size);
        
        return true;
    }

    return false;
}

bool try_to_fusion_previous(t_block* b)
{
    if (b == NULL)
        return false;

    if ((b->previous != NULL) && b->previous->free)
    {
        printf("[MALLOC INTERNAL] Merging block %p with previous one %p because both are free\n", (void*)b, (void*)b->previous);
        b = b->previous;
        b->size += b->next->size + sizeof(t_block);
        b->next = b->next->next;
        printf("[MALLOC INTERNAL] Block %p is now of size %ld.\n", (void*)b, b->size);
        return true;
    }

    return false;
}

void* calloc_perso(size_t num, size_t size)
{
    if (num == 0 || size == 0)
        return 0;

    t_block* block = malloc_perso(num * size);
    if (block == NULL)
        return NULL;

    for (unsigned int i = 0; i < num * size; i++)
        *(((char*)block) + i) = 0;

    return block;
}

void* realloc_perso(void* ptr, size_t size)
{
    pad_size(&size);

    if (ptr == NULL)
        return malloc_perso(size);

    if (size == 0)
    {
        free_perso((t_block*)ptr);
        return NULL;
    }

    t_block* block = (t_block*) ptr;
    block--;
    if (!is_valid_adress(block))
        return NULL;

    if (block->size > size)
    {
        if (!split_block(block, size))
        {
            t_block* new_block = malloc_perso(size);
            block++;

            for (unsigned int i = 0; i < size; i++)
                new_block[i] = block[i];

            free_perso(block);

            return new_block;
        }
        return ptr;
    }
    else if (block->size < size)
    {
        if (block->next != NULL && block->next->free)
        {
            if (!try_to_fusion_next(block))
            {
                t_block* new_block = malloc_perso(size);
                block++;

                for (unsigned int i = 0; i < block->size; i++)
                    new_block[i] = block[i];

                free_perso(block);

                return new_block;
            }

            return ptr;
        }
    }
    return ptr;
}