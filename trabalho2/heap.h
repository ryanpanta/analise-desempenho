#include "heap.c"

int pai(int i);
int esquerda(int i);
int direita(int i);
void heapify_up(int v);
void heapify_down(int v);
void insere(Evento valor);
void deleta(int posicao);
void printa_raiz(void);