/*          ----    anotações e explicações do professor    ----

    cada ligação tem em média 2 minutos

    vamos fazer os cálculos para uma ocupação de 60%:

    link = 1 Gbps
    60% temos que ter que os dados = 0,6 Gbps

    quantas pessoas precisam estar ativas (em média) de modo a gerarmos 0,6 Gbps?
    tamanho do pacote = 188bytes

    então, quantos pacotes uma pessoa gera por segundo? 
    cada pessoa gera 1 pacote a cada 20ms, então a cada segundo uma pessoa
    vai gerar 50 pacotes, pois 50 * 20ms = 1s;

    agora, quantas Gbps uma pessoa gera por segundo?
    temos que cada pacote é igual a 188bytes, 188*50 = 9400 bytes/s 
    9400 bytes = 75200 bits
    75200 bits = 0,000075200 Gbps

    agora, quantas pessoas precisa para dar 0,6 Gbps?
    podemos fazer uma regra de três:
    1 ----- 0,000075200 Gbps
    x ----- 0,6 Gbps
    Resolução:
    0,000075200x = 0,6
    x = 0,6/0,000075200
    x = 7978.72340426 pessoas

    última pergunta, quantas pessoas precisa gerar em média por segundo, para que 
    depois de 2 minutos, o link fica estável em 60% de ocupação ou em torno de 7978.72340426 pessoas?
    para isso, precisamos pegar o número de pessoas e dividir por 2 minutos (em segundos)
    com isso temos 7978.72340426/120 = 66.4893617021 pessoas por segundo depois de 2 minutos

    ---------------------------------------------------------------------------------------------------------

    gerar conexão -> l = 1/66.4893617021

    if(tempo == nova_conexao){           
    10s    //criar conexão --------------------> alocar (malloc)
    12s    //gerar tempo próxima conexão   \---> gerar tempo duração (2 min)
    } 
    else if(tempo == chegada_pct){
        //gera pacote-->fila roteamento/roteador
        //proximo pacote = 10,02s                               implementar árvore min_heap
        se o pacote for > 12 (free)                                         nc
    }                                                                      /  \
    else if(tempo == coleta){                                             /    \
    .                                                                    c      c
    }                                                                   / \
.                                                                     c    c



*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

// Definindo uma estrutura para armazenar parâmetros da simulação
typedef struct {
    double media_chegada;    // Taxa média de chegada de clientes
    double media_servico;    // Taxa média de serviço
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

typedef struct {
    double tempo_chegada;
    double tempo_duracao;
    // Outros campos necessários para uma conexão VoIP
} conexao;

typedef struct {
    conexao* array;
    int tamanho;
    int capacidade;
} minheap;

typedef struct {
    double tempo_chegada;
    // Outros campos necessários para um pacote VoIP
} pacote;



// Função para ler os parâmetros da simulação a partir da entrada padrão
void le_parametros(parametros * params){
    printf("Informe o tempo medio entre clientes (s): ");
    scanf("%lF", &params->media_chegada);
    params->media_chegada = 1.0/params->media_chegada;

    printf("Informe o tempo medio de servico (s): ");
    scanf("%lF", &params->media_servico);
    params->media_servico = 1.0/params->media_servico;

    printf("Informe o tempo a ser simulado (s): ");
    scanf("%lF", &params->tempo_simulacao);
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
    double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    double tempo_saida = DBL_MAX; // Inicializa o tempo de saída como infinito
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

   
    double tempo_coleta = 10.0;
    FILE *f;
    f = fopen("result.csv", "wt");

    // Loop principal da simulação
    while(tempo_decorrido < params.tempo_simulacao){
        tempo_decorrido = min(min(tempo_chegada, tempo_saida), tempo_coleta);

        if(tempo_decorrido == tempo_chegada){
            // Evento de chegada
            if(!fila){
                // Se não há clientes na fila, inicia-se o serviço imediatamente
                double tempo_servico = (-1.0/params.media_servico) * log(uniforme());
                tempo_saida = tempo_decorrido + tempo_servico; // Define o tempo de saída
                soma_ocupacao += tempo_servico;  // Atualiza a soma da ocupação
            }
            fila++; // Incrementa o número de clientes na fila
            max_fila = fila > max_fila? fila:max_fila;
            tempo_chegada = tempo_decorrido + (-1.0/params.media_chegada) * log(uniforme());  // Define o próximo tempo de chegada

            //calculo little -- E{N} -- chegada
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;

            //calculo little -- E{W} -- chegada
            e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;

        } else if(tempo_decorrido == tempo_saida){
            // Evento de saída
            fila--; // Remove um cliente da fila
            if(fila){
                // Se há clientes na fila, o próximo cliente começa o serviço
                double tempo_servico = (-1.0/params.media_servico) * log(uniforme());
                tempo_saida = tempo_decorrido + tempo_servico; // Define o tempo de saída
                soma_ocupacao += tempo_servico;  // Atualiza a soma da ocupação
            }else{
                // Se a fila está vazia, o servidor fica ocioso (tempo de saída infinito)
                tempo_saida = DBL_MAX;
            }
            
            //calculo little -- E{N}
            e_n.soma_areas += (tempo_decorrido - e_n.tempo_anterior) * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            //calculo little -- E{W} -- chegada
            e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;

        } else if(tempo_decorrido == tempo_coleta){

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
        } 

        else{
            // Tratamento de evento inválido
            printf("Evento invalido!\n");
            return(1);
        }
    }

    e_w_chegada.soma_areas += (tempo_decorrido - e_w_chegada.tempo_anterior) * e_w_chegada.no_eventos;
    e_w_saida.soma_areas += (tempo_decorrido - e_w_saida.tempo_anterior) * e_w_saida.no_eventos;

    // Cálculo e exibição da taxa média de ocupação do servidor
    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);

    printf("tamanho maximo da fila: %d\n", max_fila);

    double e_n_calculo = e_n.soma_areas / tempo_decorrido; 
    double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
    double lambda = e_w_chegada.no_eventos / tempo_decorrido;

    printf("E[N]: %lF\n", e_n_calculo);
    printf("E[W]: %lF\n", e_w_calculo);

    printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);
    fclose(f);

    return 0;
}
