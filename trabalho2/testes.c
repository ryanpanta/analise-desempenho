#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>


#define TAM_PACOTE 188.0 * 8.0 // tamanho do pacote = 188 bytes * 8
#define TEMPO_SIMULACAO 100.0 // 10 dias em segundos
//#define INVERVALO_PACOTE 0.02 // 20ms em segundos
#define TAM_LINK 1000000000.0 // 1 Gbps
//#define PACOTE_SEGUNDO 75200.0 // pacote por segundo, ou seja, um usuario manda 75200 bits por segundo
#define GBPS_USUARIO 0.0000752 // quantidade de Gbps por usuario
//#define TEMPO_SERVICO_S TAM_PACOTE/TAM_LINK // tempo de serviço para atender um pacote 
//#define TEMPO_CHAMADA 1.0 / 120.0 // tempo medio chamada

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
    l->no_eventos = 0.0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

enum TipoEvento {
    NOVA_CONEXAO, // 0 
    CONEXAO_ATIVA, // 1 envio de pacotes
    TEMPO_SERVICO, // 2 saida de pacotes
    COLETA_DADOS, // 3
};

typedef struct {
    enum TipoEvento tipo;
    double tempo;
    double tempo_duracao; // pacote (tempo de serviç)
    // outros campos necessários
} Evento;

#define MAXN 10000000


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


// Função para ler os parâmetros da simulação a partir da entrada padrão
void le_parametros(parametros * params){

    printf("Informe a ocupação: (60%%, 80%%, 90%%, 99%%)\n");
    scanf("%lF", &params->ocupacao);
    quantidade_pessoas = params->ocupacao/GBPS_USUARIO;
    quantidade_pessoas_segundo = quantidade_pessoas/120.0;
    
    printf("Tempo de simulação definido para 864000s (10 dias)\n");
    params->tempo_simulacao = TEMPO_SIMULACAO;
    
    printf("É preciso de %lF pessoas para atingir a ocupação desejada\n", quantidade_pessoas);
    printf("É preciso de %lF pessoas por segundo para atingir a ocupação desejada\n", quantidade_pessoas_segundo);
 
}

Evento criaEvento(int tipo, double tempo, double tempo_duracao){
    Evento e;
    e.tipo = tipo;
    e.tempo = tempo;
    e.tempo_duracao = tempo_duracao;

    return e;
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

int main() {

    double tempo_chamada = 1.0/120.0;
    double tempo_servico_s = (188.0 * 8.0) / 1000000000.0;

    printf("tempo servico: %lF \n", tempo_servico_s); // 188 * 8 / 1e9
    printf("tempo chamada:%lF\n", tempo_chamada);

    int semente = time(NULL);
    srand(semente);

    // Leitura dos parâmetros da simulação
    parametros params;
    le_parametros(&params);
    
    // Variáveis de controle da simulação
    double tempo_decorrido = 0.0;
    double intervalo_chegada = 1.0/quantidade_pessoas_segundo; // 60% ocupação - > 1/66.489362 -> 0.01504
    double media_chegada = 1.0/intervalo_chegada; // -> 1.0/0.01504
    double tempo_chegada = (-1.0 / media_chegada) * log(uniforme()); 
    double tempo_saida = DBL_MAX; // Inicializa o tempo de saída como infinito
    double tempo_conexao = 0.0;

    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

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
    Evento e;
    
    e = criaEvento(COLETA_DADOS, 10.0, 0.0);    
    insere(e);

    e = criaEvento(NOVA_CONEXAO, tempo_chegada, 0.0);
    insere(e);
    
    double tempo_coleta = 10.0;
    FILE *f;
    f = fopen("result.csv", "wt");

    // Loop principal da simulação
    while(tempo_decorrido < params.tempo_simulacao){
        e = heap[1]; // pega o menor valor (minimo) //
        //printa_raiz();
        deleta(1); // DELETA O MENOR DA HEAP
        tempo_decorrido = e.tempo;
        

        if(e.tipo == NOVA_CONEXAO){ 
            // Evento de chegada
            
            tempo_conexao = (-1.0/tempo_chamada) * log(uniforme()); // aproximadamente 2 minutos
            
            double tempo_conexao_ativa = tempo_decorrido + tempo_conexao;
            
            e = criaEvento(CONEXAO_ATIVA, tempo_decorrido, tempo_conexao_ativa);
            insere(e); //EVENTO DE CONEXAO ATIVA (ENVIO DE PACOTES)
             
            double nova_conexao = tempo_decorrido + (-1.0/media_chegada) * log(uniforme());
            e = criaEvento(NOVA_CONEXAO, nova_conexao, 0.0); //EVENTO DA PROXIMA CONEXAO
            insere(e);
            

        } else if(e.tipo == CONEXAO_ATIVA){ //ENVIO DE PACOTES (ENTRADA/CHEGADA)

            tempo_conexao = e.tempo_duracao;
           
            double tempo_pacote = tempo_decorrido + 0.02;
        
            if(tempo_pacote < tempo_conexao){
                e = criaEvento(CONEXAO_ATIVA, tempo_pacote, tempo_conexao);
                insere(e);
            }
        
            if(!fila){
                tempo_saida = tempo_decorrido + tempo_servico_s;
                e = criaEvento(TEMPO_SERVICO, tempo_saida, 0.0);
                insere(e);
                soma_ocupacao += tempo_servico_s;
            }

            fila++;
            max_fila = fila >= max_fila? fila: max_fila;

            //calculo little -- E{N}
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;

            //calculo little -- E{W} -- chegada
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;

        } else if(e.tipo == TEMPO_SERVICO){ // SAIDA DE PACOTE

            fila--;
            if(fila){
                tempo_saida = tempo_decorrido + tempo_servico_s;
                e = criaEvento(TEMPO_SERVICO, tempo_saida, 0.0);
                insere(e);
                soma_ocupacao += tempo_servico_s;
            }
            else{
                tempo_saida = DBL_MAX;
            }
        

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            // Calculos de Little (E[W] Saída)
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;
        
        } else if(e.tipo == COLETA_DADOS){ //c

            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.tempo_anterior = tempo_decorrido;
            
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.tempo_anterior = tempo_decorrido;
           
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.tempo_anterior = tempo_decorrido;

            double e_n_calculo = e_n.soma_areas / tempo_decorrido; 
            double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;
            double erroLittle = e_n_calculo - lambda * e_w_calculo;

            fprintf(f, "%.2lF,%.20lF\n", tempo_coleta, fabs(erroLittle));
            
            printf("Tempo de: %lF Erro de Little: %.20lF\n", tempo_coleta, fabs(erroLittle));
            tempo_coleta += 10;
            e = criaEvento(COLETA_DADOS, tempo_decorrido + 10.0, 0.0);
            insere(e);
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
    double e_n_calculo = e_n.soma_areas / tempo_decorrido; 
    double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    double lambda = e_w_chegada.no_eventos / tempo_decorrido;
    
    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);

    printf("tamanho maximo da fila: %d\n", max_fila);


    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);

    printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);
    fclose(f);

    return 0;
}
