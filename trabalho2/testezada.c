#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>

// Evento

// 'n': Nova Conexão (indica o momento quando o próximo cliente estabelecerá conexão com o roteador)
// 's': Tempo de Serviço (indica quando o pacote atual será enviado e o roteador ficará novamente disponível)
// 'p': Tempo de Pacote (indica o momento em que um pacote é enviado para a fila do roteador)
// 'c': Tempo de Coleta (intervalo contínuo de 10 segundos para a coleta dos dados e cálculo de Little)

struct evento{
    char tipo_evento;
    double tempo_evento;
    double tempo_limite; // Exclusivo de eventos "Tempo de Pacote"
};

struct evento cria_evento(char tipo, double tempo, double limite){
    struct evento novo_evento;

    novo_evento.tipo_evento = tipo;
    novo_evento.tempo_evento = tempo;
    novo_evento.tempo_limite = limite;

    return novo_evento;
}

// Heap

struct heap{
    struct evento * array;
    int size;
    int capacity;
};

// Funções Auxiliares

void heapify(struct heap * heap, int index){
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    if(left >= heap->size || left < 0) left = -1;
    if(right >= heap->size || right < 0) right = -1;

    if(left != -1 && heap->array[left].tempo_evento < heap->array[index].tempo_evento) min = left;
    if(right != -1 && heap->array[right].tempo_evento < heap->array[min].tempo_evento) min = right;

    if(min != index){
        struct evento aux = heap->array[min];
        heap->array[min] = heap->array[index];
        heap->array[index] = aux;

        heapify(heap, min);
    }
}

void organiza_heap(struct heap * heap, int index){
    int parent = (index - 1) / 2;

    if(heap->array[parent].tempo_evento > heap->array[index].tempo_evento){
        struct evento aux = heap->array[parent];
        heap->array[parent] = heap->array[index];
        heap->array[index] = aux;

        organiza_heap(heap, parent);
    }
}

// Funções Heap

struct heap * cria_heap(int capacity){
    struct heap * newHeap = (struct heap*)malloc(sizeof(struct heap));

    if(newHeap == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    newHeap->size = 0;
    newHeap->capacity = capacity;
    newHeap->array = (struct evento *)malloc(capacity * sizeof(struct evento));

    if(newHeap->array == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    return newHeap;
}

void insere_heap(struct heap * heap, struct evento novo_evento){
    if(heap->size >= heap->capacity) return;
    
    heap->array[heap->size] = novo_evento;
    organiza_heap(heap, heap->size);
    heap->size++;
}

struct evento extrai_heap(struct heap * heap){
    if(heap->size == 0){
        puts("A heap esta vazia!");
        struct evento evento_vazio;
        return evento_vazio;
    }

    struct evento evento_extraido = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    heapify(heap, 0);
    return evento_extraido;
}

void imprime_heap(struct heap * heap){
    for(int i = 0; i < heap->size; i++)
        printf("[%c]: %lF,  %lF\n", heap->array[i].tipo_evento, heap->array[i].tempo_evento, heap->array[i].tempo_limite);
}

// Entrada

typedef struct {
    double media_chegada;
    double tempo_simulacao;
} parametros;

void le_parametros(parametros * params){
    printf("Informe o tempo medio entre clientes (s): ");
    scanf("%lF", &params->media_chegada);
    params->media_chegada = 1.0/params->media_chegada;

    printf("Informe o tempo a ser simulado (s): ");
    scanf("%lF", &params->tempo_simulacao);
}

// Analise

typedef struct {
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

void inicia_little(little * l){
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

// Simulador

double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1); //limitando entre (0,1]

	return (1.0 - u);
}

int main(){
    // RNG

    int semente = time(NULL);
    srand(semente);

    // Input

    parametros params;
    le_parametros(&params);

    // Parametros fixos
    double tamanho_pacote = 188.0 * 8.0; // 188 Bytes
    double largura_de_banda = 1000000000.0; // 1 Gigabit / segundo
    double tempo_servico = tamanho_pacote / largura_de_banda; // Tempo de processamento de cada pacote
    
    double tempo_injecao = 0.020; // 20 milisegundos
    double media_conexao = 120.0; // 2 minutos

    // Arvore de Eventos

    // Árvore Heap de Eventos
    int maximo_arvore = 1000000; // Tamanho máximo suportado pela Árvore de Eventos
    struct evento evento_atual;
    struct heap * arvore_de_eventos = cria_heap(maximo_arvore);

    // Primeira Coleta
    evento_atual = cria_evento('c', 0.0, 0.0);
    insere_heap(arvore_de_eventos, evento_atual);

    // Primeira Conexão
    double tempo_chegada = (-1.0 / params.media_chegada) * log(uniforme());
    evento_atual = cria_evento('n', tempo_chegada, 0.0);
    insere_heap(arvore_de_eventos, evento_atual);

    // Controle de Simulação
    
    double tempo_decorrido = 0.0;

    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    double tempo_conexao;
    
    // Medidas de Interesse

    double soma_ocupacao = 0.0;

    // Little

    little e_n;
    inicia_little(&e_n);

    little e_w_chegada;
    inicia_little(&e_w_chegada);
    
    little e_w_saida;
    inicia_little(&e_w_saida);
    
    while(1){
        evento_atual = extrai_heap(arvore_de_eventos);
        tempo_decorrido = evento_atual.tempo_evento;
        printf("%lF\n", tempo_decorrido);

        if(tempo_decorrido >= params.tempo_simulacao) break;

        switch(evento_atual.tipo_evento){
            case 'n': // Nova Conexão
                // Insere a nova conexão na heap
                tempo_conexao = (-1.0 / media_conexao) * log(uniforme());
                evento_atual = cria_evento('p', tempo_decorrido, tempo_decorrido + tempo_conexao);
                insere_heap(arvore_de_eventos, evento_atual);

                // Define o momento da próxima conexão
                tempo_chegada = (-1.0 / params.media_chegada) * log(uniforme());
                evento_atual = cria_evento('n', tempo_decorrido + tempo_chegada, 0.0);
                insere_heap(arvore_de_eventos, evento_atual);

                break;
            
            case 's': // Tempo de Serviço
                // Reduz a fila
                fila--;

                if(fila){
                    // Processa o próximo pacote (fila não vazia)
                    evento_atual = cria_evento('s', tempo_decorrido + tempo_servico, 0.0);
                    insere_heap(arvore_de_eventos, evento_atual);
                }

                // Calculos de Little (E[N])
                e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
                e_n.no_eventos--;
                e_n.tempo_anterior = tempo_decorrido;

                // Calculos de Little (E[W] Saída)
                e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
                e_w_saida.no_eventos++;
                e_w_saida.tempo_anterior = tempo_decorrido;

                break;
            
            case 'p': // Tempo de Pacote
                // Aumenta a fila
                fila++;
                if(fila > max_fila) max_fila = fila;
                
                if(!fila){
                    // Define o tempo de serviço (fila vazia)
                    evento_atual = cria_evento('s', tempo_decorrido + tempo_servico, 0.0);
                    insere_heap(arvore_de_eventos, evento_atual);
                }

                if(tempo_decorrido + tempo_injecao <= evento_atual.tempo_limite){
                    // Insere o próximo pacote da conexão na heap (caso a conexão persista)
                    tempo_conexao = evento_atual.tempo_limite;
                    evento_atual = cria_evento('p', tempo_decorrido + tempo_injecao, tempo_conexao);
                    insere_heap(arvore_de_eventos, evento_atual);
                }

                // Calculos de Little (E[N])
                e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
                e_n.no_eventos++;
                e_n.tempo_anterior = tempo_decorrido;

                // Calculos de Little (E[W] Chegada)
                e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
                e_w_chegada.no_eventos++;
                e_w_chegada.tempo_anterior = tempo_decorrido;

                break;

            case 'c': // Tempo de Coleta
                // Calculos de Little
                e_n.soma_areas += e_n.no_eventos * (tempo_decorrido - e_n.tempo_anterior);
                e_n.tempo_anterior = tempo_decorrido;

                e_w_chegada.soma_areas += e_w_chegada.no_eventos * (tempo_decorrido - e_w_chegada.tempo_anterior);
                e_w_chegada.tempo_anterior = tempo_decorrido;
                
                e_w_saida.soma_areas += e_w_saida.no_eventos * (tempo_decorrido - e_w_saida.tempo_anterior);
                e_w_saida.tempo_anterior = tempo_decorrido;

                double e_n_calculo = e_n.soma_areas / tempo_decorrido;
                double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
                double lambda = e_w_chegada.no_eventos / tempo_decorrido;
                
                // Imprime os resultados
                printf("%.0lF, %.20lF\n", tempo_decorrido, e_n_calculo - lambda * e_w_calculo);

                // Insere o novo valor de calculo na heap
                evento_atual = cria_evento('c', tempo_decorrido + 10.0, 0.0);
                insere_heap(arvore_de_eventos, evento_atual);

                break;

            default:
                puts("Evento invalido!");
                return 1;
        }
    }
    
    // Calculos Finais

    e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
    e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    double e_n_calculo = e_n.soma_areas / tempo_decorrido;
    double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    double lambda = e_w_chegada.no_eventos / tempo_decorrido;

    printf("Tempo de Serviço: %lF\n", tempo_servico);

    printf("Taxa de Ocupacao: %lF\n", (soma_ocupacao/tempo_decorrido));
    printf("Tamanho maximo da fila: %ld\n", max_fila);

    printf("E[N]: %lF\n", e_n_calculo);    
    printf("E[W]: %lF\n", e_w_calculo);
    printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);
        
    return 0;
}