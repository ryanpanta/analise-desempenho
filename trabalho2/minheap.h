#include "minheap.c"

heap* createHeap(int capacity, int* nums);
void insertHelper(heap* h, int index);
void heapify(heap* h, int index);
int extractMin(heap* h);
void insert(heap* h, int data);
