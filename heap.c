#ifndef _HEAP
#include "heap.h"
#endif

typedef struct heap_item
{
    int index;
    int value;
    struct heap_item *child_1;
    struct heap_item *child_2;
} HEAP_ITEM;

HEAP_ITEM *heap = 0;

ERROR_CODE fetch(int index, int64_t *item)
{
    HEAP_ITEM *curr_entry = heap;

    /* Find heap item */
    while (curr_entry != 0 && curr_entry->index != index)
    {
        if (index > curr_entry->index)
        {
            curr_entry = curr_entry->child_1;
        }
        else
        {
            curr_entry = curr_entry->child_2;
        }
    }

    /* Error if no heap entry */
    if (curr_entry == 0)
    {
        return null_heap_entry;
    }
    *item = curr_entry->value;

    return no_error;
}

void store(int index, int64_t item)
{
    HEAP_ITEM *curr_entry = heap;

    if (heap == 0)
    {
        heap = calloc(1, sizeof(HEAP_ITEM));
        heap->index = index;
        curr_entry = heap;
    }
    while (index != curr_entry->index)
    {
        if (index > curr_entry->index)
        {
            if (curr_entry->child_1 == 0)
            {
                curr_entry->child_1 = calloc(1, sizeof(HEAP_ITEM));
                curr_entry->child_1->index = index;
            }
            curr_entry = curr_entry->child_1;
        }
        else
        {
            if (curr_entry->child_2 == 0)
            {
                curr_entry->child_2 = calloc(1, sizeof(HEAP_ITEM));
                curr_entry->child_2->index = index;
            }
            curr_entry = curr_entry->child_2;
        }
    }
    curr_entry->value = item;

    return;
}

void print_heap(HEAP_ITEM *item)
{
    if (item != 0)
    {
        if (item->child_2 != 0)
        {
            print_heap(item->child_2);
        }
        printf("%4d %d\n", item->index, item->value);
        if (item->child_1 != 0)
        {
            print_heap(item->child_1);
        }
    }

    return;
}

void heap_dump()
{
    printf("Heap Dump:\n");
    print_heap(heap);

    return;
}
