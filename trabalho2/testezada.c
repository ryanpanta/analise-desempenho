
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "minheap.h"

#define TAM_PACOTE 1504
#define TEMPO_SIMULACAO 864000
#define INVERVALO_PACOTE 0.02
#define TAM_LINK 1000000000
#define PACOTE_SEGUNDO 75200
#define GBPS_USUARIO 0.0000752

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

typedef struct {
    double tempo_chegada;
    double tempo_duracao;
    // mais
} conexao;

// typedef struct {
//     conexao* array;
//     int tamanho;
//     int capacidade;
// } minheap;

typedef struct {
    double tempo_chegada;
    // mais
} pacote;



// Função para ler os parâmetros da simulação a partir da entrada padrão
void le_parametros(parametros * params){

    printf("Informe a ocupação: (60%%, 80%%, 90%%, 99%%)\n");
    scanf("%lF", &params->ocupacao);
    double quantidade_pessoas = params->ocupacao/GBPS_USUARIO;
    
    printf("Tempo de simulação definido para 864000s (10 dias)\n");
    params->tempo_simulacao = TEMPO_SIMULACAO;
    
    printf("É preciso de %lF pessoas para atingir a ocupação desejada\n", quantidade_pessoas);
 
}


// Função para gerar um número aleatório uni

int main(){
    parametros param;
    le_parametros(&param);
}