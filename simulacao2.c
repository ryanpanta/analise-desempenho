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

        } else{
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

    printf("Erro de Little: %lF\n", e_n_calculo - lambda * e_w_calculo);


    return 0;
}
