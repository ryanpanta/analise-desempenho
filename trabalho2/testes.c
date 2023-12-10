#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>


#define TAM_PACOTE 188.0 * 9.0 // tamanho do pacote = 188 bytes * 8
#define TEMPO_SIMULACAO 100.0 // 10 dias em segundos
#define INVERVALO_PACOTE 0.02 // 20ms em segundos
#define TAM_LINK 1000000000.0 // 1 Gbps
#define PACOTE_SEGUNDO 75200.0 // pacote por segundo, ou seja, um usuario manda 75200 bits por segundo
#define GBPS_USUARIO 0.0000752 // quantidade de Gbps por usuario
#define TEMPO_SERVICO_S TAM_PACOTE/TAM_LINK // tempo de serviço para atender um pacote
#define TEMPO_CHAMADA 120.0

double quantidade_pessoas = 0.0;
double quantidade_pessoas_segundo = 0.0;

// Definindo uma estrutura para armazenar parâmetros da simulação
typedef struct {
    double ocupacao;          // Taxa média de chegada de clientes;
    double tempo_simulacao;  // Tempo total de simulação
} parametros;

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

// Declare a heap structure
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



// Função para ler os parâmetros da simulação a partir da entrada padrão
void le_parametros(parametros * params){

    printf("Informe a ocupação: (60%%, 80%%, 90%%, 99%%)\n");
    scanf("%lF", &params->ocupacao);
    quantidade_pessoas = params->ocupacao/GBPS_USUARIO;
    quantidade_pessoas_segundo = quantidade_pessoas/120.0;
    
    printf("Tempo de simulação definido para 864000s (10 dias)\n");
    params->tempo_simulacao = TEMPO_SIMULACAO;
    
    printf("É preciso de %lF pessoas para atingir a ocupação desejada\n", quantidade_pessoas);
 
}

// Função para gerar um número aleatório uniformemente distribuído entre 0 e 1
double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1);
	// Limitando entre (0,1]
	u = 1.0 - u;

	return (u);
}

// Função para retornar o valor mínimo entre dois números
double min(double d1, double d2){
    if(d1 < d2) return d1;
    return d2;
}



int main(){

    int semente = time(NULL);
    srand(semente);

    // Leitura dos parâmetros da simulação
    parametros params;
    le_parametros(&params);


    // Variáveis de controle da simulação
    double tempo_decorrido = 0.0;
    double intervalo_chegada = 1/quantidade_pessoas_segundo; // 1/66
    double tempo_chegada = (-1.0 / (1.0/ intervalo_chegada)) * log(uniforme()); //primeira conexão
    double tempo_saida = DBL_MAX; // Inicializa o tempo de saída como infinito
    double tempo_conexao = 0.0;

    unsigned long int fila = 0;
    int max_fila = 0.0;

    // Variável para medir a ocupação do servidor
    double soma_ocupacao = 0.0;
    /*
        Little
    */
    little e_n;
    little e_w_chegada;
    little e_w_saida;

    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);
    /*
        Little - fim
    */

    heap* eventos_heap = createHeap(100000);
    Evento e;
    
    insert(eventos_heap, novo_evento(COLETA_DADOS, 10, 0.0));
    insert(eventos_heap, novo_evento(NOVA_CONEXAO, tempo_chegada, 0.0));
    
    
    double tempo_coleta = 10.0;
    FILE *f;
    f = fopen("result.csv", "wt");

    // Loop principal da simulação
    while(tempo_decorrido < params.tempo_simulacao){
        e = getMin(eventos_heap);
        //freeEvento(&e);
        tempo_decorrido = e.tempo;
        printf("%lF\n", tempo_decorrido); //  <--- testes

        if(e.tipo == NOVA_CONEXAO){ //n
            // Evento de chegada
               
            tempo_conexao = (-1.0/TEMPO_CHAMADA) * log(uniforme());
            // isso deve manter? tempo_saida = tempo_decorrido + TEMPO_SERVICO_S; // Define o tempo de saída
            
            //soma_ocupacao += TEMPO_SERVICO_S;  
            
            double tempo_saida_conexao = tempo_decorrido + tempo_conexao;

            double tempo_conexao_ativa = tempo_decorrido + tempo_conexao;
            insert(eventos_heap, novo_evento(CONEXAO_ATIVA, tempo_conexao_ativa, tempo_saida_conexao));
        
            double nova_conexao = tempo_decorrido + (-1.0/quantidade_pessoas_segundo) * log(uniforme());
            insert(eventos_heap, novo_evento(NOVA_CONEXAO, nova_conexao, 0.0));
            
            //max_fila = fila > max_fila? fila:max_fila;
            // isso deve manter? tempo_chegada = tempo_decorrido + (-1.0/params.media_chegada) * log(uniforme());  // Define o próximo tempo de chegada

            //calculo little -- E{N} -- chegada
            // e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            // e_n.no_eventos++;
            // e_n.tempo_anterior = tempo_decorrido;

            // //calculo little -- E{W} -- chegada
            // e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            // e_w_chegada.no_eventos++;
            // e_w_chegada.tempo_anterior = tempo_decorrido;

        } else if(e.tipo == CONEXAO_ATIVA){ //p
            // Evento de saída

            fila++;
            if(fila > max_fila) max_fila = fila;
            double tempo_pacote = tempo_decorrido;
            tempo_pacote += 0.02;
            
            double tempo_saida_servico = tempo_decorrido + TEMPO_SERVICO_S;

            if(tempo_pacote > tempo_saida_servico){
                freeEvento(&e);
            }
            else
                insert(eventos_heap, novo_evento(CONEXAO_ATIVA, tempo_pacote, tempo_saida_servico));


            if(!fila){
                tempo_saida = tempo_decorrido + TEMPO_SERVICO_S;
                insert(eventos_heap, novo_evento(TEMPO_SERVICO, TEMPO_SERVICO_S, tempo_saida));
                soma_ocupacao += TEMPO_SERVICO_S;
            }
            // fila--; // Remove um cliente da fila
            // if(fila){
            //     // Se há clientes na fila, o próximo cliente começa o serviço
                
            //     tempo_saida = tempo_decorrido + TEMPO_SERVICO_S; // Define o tempo de saída
            //     soma_ocupacao += TEMPO_SERVICO_S;  // Atualiza a soma da ocupação
            // }else{
            //     // Se a fila está vazia, o servidor fica ocioso (tempo de saída infinito)
            //     tempo_saida = DBL_MAX;
            // }
            
            //calculo little -- E{N}
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            //calculo little -- E{W} -- chegada
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;

        } else if(e.tipo == TEMPO_SERVICO){ //s

            fila--;
            if(fila){
                tempo_saida = tempo_decorrido + TEMPO_SERVICO_S;
                insert(eventos_heap, novo_evento(TEMPO_SERVICO, tempo_saida, 0.0));
                soma_ocupacao += TEMPO_SERVICO_S;
            }else{
                tempo_saida = DBL_MAX;
            }

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculos de Little (E[W] Saída)
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;
        
        } else if(tempo_decorrido == COLETA_DADOS){ //c

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            e_w_chegada.tempo_anterior = tempo_decorrido;
            e_w_saida.tempo_anterior = tempo_decorrido;

            double e_n_calculo = e_n.soma_areas / tempo_decorrido; 
            double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;
            double erroLittle = e_n_calculo - lambda * e_w_calculo;

            fprintf(f, "%.2lF,%.20lF\n", tempo_coleta, fabs(erroLittle));
            
            printf("Tempo de: %lF Erro de Little: %.20lF\n", tempo_coleta, fabs(erroLittle));
            tempo_coleta += 10;
            insert(eventos_heap, novo_evento(COLETA_DADOS, tempo_decorrido + 10.0, 0.0));
        } 

        else{
            // Tratamento de evento inválido
            printf("Evento invalido!\n");
            return(1);
        }
    }

    e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
    e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    //Cálculo e exibição da taxa média de ocupação do servidor
    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);

    printf("tamanho maximo da fila: %d\n", max_fila);

    double e_n_calculo = e_n.soma_areas / tempo_decorrido; 
    double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    double lambda = e_w_chegada.no_eventos / tempo_decorrido;

    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);

    printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);
    fclose(f);

    free(eventos_heap->arr);
    free(eventos_heap);

    return 0;
}
