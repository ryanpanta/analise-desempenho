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

    // Loop principal da simulação
    while(tempo_decorrido < params.tempo_simulacao){
        tempo_decorrido = min(tempo_chegada, tempo_saida);

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
        } else{
            // Tratamento de evento inválido
            printf("Evento invalido!\n");
            return(1);
        }
    }

    // Cálculo e exibição da taxa média de ocupação do servidor
    printf("ocupacao: %lF\n", soma_ocupacao/tempo_decorrido);

    printf("tamanho maximo da fila: %d\n", max_fila);

    return 0;
}
