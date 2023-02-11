#pragma once
#include <stdbool.h>
#include <stddef.h>

void* g_head;
void* g_free_list;

typedef struct s_block
{
    size_t size;
    bool free;
    struct s_block* next;
    struct s_block* previous;
}t_block;


void initialize_block(t_block* block);

void insert_block(t_block* block, void** list);

t_block* expend_heap(size_t size);

void* malloc_perso(size_t size);

void pad_size(size_t* size);

bool is_valid_adress(t_block* block);

t_block* find_block(size_t size);

bool split_block(t_block* b, size_t size);

bool try_to_fusion(t_block* b);

bool try_to_fusion_next(t_block* b);

bool try_to_fusion_previous(t_block* b);

void* calloc_perso( size_t num, size_t size );

void* realloc_perso(void* ptr, size_t size);