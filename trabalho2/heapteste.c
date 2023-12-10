#include <stdio.h>
#include <stdlib.h>


enum TipoEvento {
    NOVA_CONEXAO, 
    CONEXAO_ATIVA,
    TEMPO_SERVICO,
    COLETA_DADOS
};

typedef struct {
    enum TipoEvento tipo;
    double tempo;
    double tempo_duracao; // pacote (tempo de serviç)
    // outros campos necessários
} Evento;

Evento novo_evento(int tipo, double tempo, double tempo_duracao) {
    Evento e;

    e.tipo = tipo;
    e.tempo = tempo;
    e.tempo_duracao = tempo_duracao;

    return e;
}

struct Heap {
	Evento* arr;
	int size;
	int capacity;
};

// define the struct Heap name
typedef struct Heap heap;


// Define a createHeap function
heap* createHeap(int capacity) {
    heap* h = (heap*)malloc(sizeof(heap));
    if (h == NULL) {
        printf("Memory error");
        return NULL;
    }
    h->size = 0;
    h->capacity = capacity;
    h->arr = (Evento*)malloc(capacity * sizeof(Evento));
    if (h->arr == NULL) {
        printf("Memory error");
        return NULL;
    }
    return h;
}

// Defining insertHelper function
void insertHelper(heap* h, int index) { //OK

    // Store parent of element at index
    // in parent variable
    int parent = (index - 1) / 2;

    if (h->arr[parent].tempo > h->arr[index].tempo) {
        // Swapping when child is smaller
        // than parent element
        Evento temp = h->arr[parent];
        h->arr[parent] = h->arr[index];
        h->arr[index] = temp;

        // Recursively calling insertHelper
        insertHelper(h, parent);
    }
}

void heapify(heap* h, int index) {

    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    // Checking whether our left or child element
    // is at right index or not to avoid index error
    if (left >= h->size || left < 0)
        left = -1;
    if (right >= h->size || right < 0)
        right = -1;

    // store left or right element in min if
    // any of these is smaller that its parent
    if (left != -1 && h->arr[left].tempo < h->arr[index].tempo)
        min = left;
    if (right != -1 && h->arr[right].tempo < h->arr[min].tempo)
        min = right;

    // Swapping the nodes
    if (min != index) {
        Evento temp = h->arr[min];
        h->arr[min] = h->arr[index];
        h->arr[index] = temp;

        // recursively calling for their child elements
        // to maintain min heap
        heapify(h, min);
    }
}


Evento extractMin(heap* h)
{
	Evento deleteItem;

	// Checking if the heap is empty or not
	if (h->size == 0) {
		printf("\nHeap id empty.");
		return (Evento){NOVA_CONEXAO, -999};
	}

	// Store the node in deleteItem that
	// is to be deleted.    
	deleteItem = h->arr[0];

	// Replace the deleted node with the last node
	h->arr[0] = h->arr[h->size - 1];
	// Decrement the size of heap
	h->size--;

	// Call minheapify_top_down for 0th index
	// to maintain the heap property
	heapify(h, 0);
	return deleteItem; 
    
}

Evento getMin(heap* h){
    if(h->size == 0){
        printf("\nHeap is empty.");
        Evento void_event;
        return void_event;
    }

    Evento evento_extraido = h->arr[0]; 
    h->arr[0] = h->arr[h->size - 1];
    h->size--;

    heapify(h, 0);
    return evento_extraido;
}

void imprime_heap(heap * h){
    for(int i = 0; i < h->size; i++)
        printf("[%c]: %lF,  %lF\n", h->arr[i].tipo, h->arr[i].tempo, h->arr[i].tempo_duracao);
}


// Define a insert function
void insert(heap* h, Evento data)
{

	// Checking if heap is full or not
	if (h->size >= h->capacity) return;
		// Inserting data into an array
    h->arr[h->size] = data;
    // Calling insertHelper function
    insertHelper(h, h->size);
    // Incrementing size of array
    h->size++;
	
}

void freeEvento(Evento *e) {
    free(e);
}

int main(){
    heap* eventos_heap = createHeap(1000000);
    Evento e;
    
    insert(eventos_heap, novo_evento(COLETA_DADOS, 10, 0.0));
    insert(eventos_heap, novo_evento(NOVA_CONEXAO, 4, 0.0));
    insert(eventos_heap, novo_evento(NOVA_CONEXAO, 2, 0.0));
    insert(eventos_heap, novo_evento(NOVA_CONEXAO, 9, 0.0));

    e = getMin(eventos_heap);
    printf("%lF ", e.tempo);
    //imprime_heap(eventos_heap->arr);
    e = getMin(eventos_heap);
    printf("%lF ", e.tempo);
    //imprime_heap(eventos_heap->);
    e = getMin(eventos_heap);
    printf("%lF ", e.tipo);
    //imprime_heap(eventos_heap->arr);
    e = getMin(eventos_heap);
    printf("%lF ", e.tipo);
    //imprime_heap(eventos_heap->arr);

}