#include <stdio.h>
#include <stdlib.h> // malloc, realloc

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index);


/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index);

////////////////////////////////////////////////////////////////////////////////
static void _reheapUp( HEAP *heap, int index){
    /* Parent: (i-1)/2
    */
    while(index != 0 && heap->compare(heap->heapArr[index], heap->heapArr[(index-1)/2]) > 0)
    {
        // exchange node and parent
        void *tmp;
        tmp = heap->heapArr[index];
        heap->heapArr[index] = heap->heapArr[(index-1)/2];
        heap->heapArr[(index-1)/2] = tmp;
        index = (index-1)/2;
    }

}

static void _reheapDown( HEAP *heap, int index){
    /* Left child: 2i + 1, Right child: 2i + 2
    */
    int left = 2*index + 1;
    int right = 2*index + 2;
    int large = 0;
    void *leftsub;
    void *rightsub;

    if(left <= heap->last){
        leftsub = heap->heapArr[left];

        if(right <= heap->last){
            rightsub = heap->heapArr[right];

            if(heap->compare(leftsub, rightsub) > 0)
                large = left;

            else large = right;
        }

        else large = left;

        if(heap->compare(heap->heapArr[index], heap->heapArr[large]) < 0)
        {
            //exchange root and large
            void *tmp;
            tmp = heap->heapArr[index];
            heap->heapArr[index] = heap->heapArr[large];
            heap->heapArr[large] = tmp;
            _reheapDown(heap,large);
        }
    }
}

HEAP *heap_Create( int capacity, int (*compare) (void *arg1, void *arg2)){
    HEAP *heap = (HEAP*)malloc(sizeof(HEAP));

    if(heap == NULL) return NULL;

    heap->heapArr = (void**)malloc(capacity * sizeof(void*));
    heap->last = -1;
    heap->capacity = capacity;
    heap->compare = compare;

    return heap;

}

void heap_Destroy( HEAP *heap){
    /*
    for(int i = 0; i < heap->capacity; i++)
        free(heap->heapArr[i]);

    free(heap->heapArr);
    free(heap);
    */
    void *dataOutPtr;

    while(heap->last > -1){
        dataOutPtr = heap->heapArr[heap->last];
        heap->last--;
        free(dataOutPtr);
    }

    free(heap->heapArr);
    free(heap);
}

int heap_Insert( HEAP *heap, void *dataPtr){
    // realloc -> capacity * 2
    if(heap->last + 1 >= heap->capacity){
        heap->capacity *= 2;
        heap->heapArr = (void**)realloc(heap->heapArr, heap->capacity * sizeof(void*));
    }

    heap->heapArr[heap->last + 1] = dataPtr;
    _reheapUp(heap, heap->last + 1);
    heap->last++;

    return 1;

}

int heap_Delete( HEAP *heap, void **dataOutPtr){
    // root to be deleted -> dataOutPtr
    if(heap_Empty(heap)) return 0;

    *dataOutPtr = heap->heapArr[0];
    heap->heapArr[0] = heap->heapArr[heap->last];
    heap->last--;
    _reheapDown(heap, 0);

    return 1;

}

int heap_Empty(  HEAP *heap){
    // Heap->last : -1
    if(heap->last == -1) return 1;
    else return 0;
}

void heap_Print( HEAP *heap, void (*print_func) (void *data)){
    for(int i = 0; i < heap->last + 1; i++)
        print_func(heap->heapArr[i]);

    printf("\n");
}
