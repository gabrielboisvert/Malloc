#include "free_perso.h"
#include "malloc_perso.h"
#include <stdbool.h>
#include <stdio.h>

void free_perso(void* ptr)
{
    if (ptr == NULL)
        return;

    t_block* block = (t_block*)ptr;
    block--;

    if (!is_valid_adress(block))
    {
        printf("[MALLOC INTERNAL] %p is not a block allocated by malloc_perso\n", (void*)ptr);
        return;
    }
    
    printf("%ld\n", block->size);

    block->free = true;

    if (block->previous != NULL)
        block->previous->next = block->next;

    if (block->next != NULL)
        block->next->previous = block->previous;

    //block->next = NULL;
    //block->previous = NULL;

    insert_block(block, &g_free_list);

    printf("[MALLOC INTERNAL] Freeing block %p\n", (void*)block);

    try_to_fusion(block);
}