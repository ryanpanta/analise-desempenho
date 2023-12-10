#include <stdio.h>
#define MAXN 1000100

typedef struct {
    enum TipoEvento tipo;
    double tempo;
    double tempo_duracao; // pacote (tempo de serviç)
    // outros campos necessários
} Evento;

Evento heap[MAXN];
int tamanho_heap;

int pai(int i)
{
    return i / 2;
}

int esquerda(int i)
{
    return 2 * i;
}

int direita(int i)
{
    return 2 * i + 1;
}

void heapify_up(int v)
{
    if (v == 1)
        return;

    int p = pai(v);
    if (heap[v].tempo < heap[p].tempo)
    {
        Evento temp = heap[v];
        heap[v] = heap[p];
        heap[p] = temp;

        heapify_up(p);
    }
}

void heapify_down(int v)
{
    int d = direita(v);
    int e = esquerda(v);

    int menor = v;

    if (d <= tamanho_heap && heap[d].tempo < heap[menor].tempo)
        menor = d;
    if (e <= tamanho_heap && heap[e].tempo < heap[menor].tempo)
        menor = e;

    if (menor != v)
    {
        Evento temp = heap[v];
        heap[v] = heap[menor];
        heap[menor] = temp;

        heapify_down(menor);
    }
}

void insere(Evento valor)
{
    heap[++tamanho_heap] = valor;

    heapify_up(tamanho_heap);
}

void deleta(int posicao)
{
    Evento temp = heap[posicao];
    heap[posicao] = heap[tamanho_heap];
    heap[tamanho_heap] = temp;

    tamanho_heap--;

    heapify_down(posicao);
}

void printa_raiz(void) {
    printf("Raiz: Evento %d tempo %.10lF\n", heap[1].tipo, heap[1].tempo);
}