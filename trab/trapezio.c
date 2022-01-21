/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Paralelização da integração numérica pelo método do trapézio.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "../lab02/timer.h"
#include "testfunctions.h"
#include "helperfunctions.h"

int n_threads[6] = { 1, 2, 4, 6, 8, 10 };
int n_subintervals[3] = { 1000, 10000, 100000 };
int n_runs = 5;

testfoo_ptr test_functions[3] =
{
    test_f1, test_f2, test_f3
};

testfoo_ptr test_primitives[3] =
{
    primitive_f1, primitive_f2, primitive_f3
};

testfoo_ptr test_derivatives[3] =
{
    derivative_f1, derivative_f2, derivative_f3
};

typedef struct
{
    int thread_id;
    int n_threads;
    int function_id;
    int n_subintervals;
} thread_args;

double min_x = 1.0;
double max_x = 11.0;  // integração em [1, 11]

void* integrate_subinterval(void* arg);

// Fluxo da thread principal:

int main(int argc, char *argv[])
{
    puts("[main] Iniciando execucao");

    pthread_t* tid;  // identificadores internos das threads
    double seq_out, conc_out;  // resultados sequencial e concorrente
    double* thread_out;  // recebe a saída de cada thread
    double analytical, h;  // p/ comparar conc e "analítico"

    double delta = pow(10, -10);  // p/ comparar valores double

    // P/ marcação de tempo:
    double t0, tf, dt;  // instantes inicial e final, e duração
    
    // (definir arrays multi p/ guardar medições de tempo)

    for (int f_idx = 0; f_idx < 3; f_idx++)  // p/ cada função de teste
    {
        for (int n_idx = 0; n_idx < 3; n_idx++) // p/ cada qtd de subintervalos
        {
            for (int r = 0; r < n_runs; r++) // p/ cada repetição da medição
            {
                // medir seq aqui

                for (int t_idx = 0; t_idx < 6; t_idx++)  // p/ cada qtd de threads
                {
                    int max_t = n_threads[t_idx];
                    conc_out = 0;  // acumulador

                    // Criando as threads:

                    tid = (pthread_t*) safe_malloc(sizeof(pthread_t*) * max_t);

                    for (int t = 0; t < max_t; t++)
                    {
                        thread_args* curr_args;

                        curr_args = (thread_args*) safe_malloc(sizeof(thread_args));

                        curr_args->thread_id = t;
                        printf("t = %d\n", t);
                        printf("curr_args->thread_id = %d\n", curr_args->thread_id);
                        curr_args->n_threads = n_threads[t_idx];
                        curr_args->function_id = f_idx;
                        curr_args->n_subintervals = n_subintervals[n_idx];

                        if (pthread_create(tid + t, NULL, integrate_subinterval, (void*) curr_args))
                        {
                            fprintf(stderr, "Falha na criacao das threads");
                            return EXIT_FAILURE;
                        }

                    }

                    // Sincronizando as threads:

                    for (int t = 0; t < max_t; t++)
                    {
                        if (pthread_join(*(tid + t), (void**) &thread_out))
                        {
                            fprintf(stderr, "Falha na sincronizacao das threads");
                            return EXIT_FAILURE;
                        }

                        conc_out += *(thread_out);

                        free(thread_out);  // precisa? alocada na tarefa da thread
                    }
                    
                    free(tid);

                    // Verificação da corretude:

                    char case_string[100];

                    sprintf
                    (
                        case_string,
                        "no caso da funcao %d com %d subintervalos e %d threads",
                        f_idx + 1,
                        n_subintervals[n_idx],
                        n_threads[t_idx]
                    );

                    // Comparando resultados sequencial e concorrente:

                    seq_out = 1.0;  // forçando erro para testar o teste

                    if (!compare_doubles(seq_out, conc_out, delta))
                    {
                        printf("[main] Erro %s,\n", case_string);
                        puts("[main] na comparacao com o resultado sequencial:");
                        printf("[main] Saida concorrente: %f\n", conc_out);
                        printf("[main] Saida sequencial: %f\n", seq_out);
                        printf("[main] (Diferença absoluta: %f)\n", fabs(seq_out - conc_out));

                        puts("");

                    }

                    /* Comparando resultado concorrente com a integral
                    calculada a partir da primitiva analítica, somada ao
                    maior termo do erro de aproximação esperado: */

                    analytical = test_primitives[f_idx](max_x) - test_primitives[f_idx](min_x);

                    h = (max_x - min_x) / n_subintervals[n_idx];
                    // comprimento do subintervalo

                    analytical += pow(h, 2) * (test_derivatives[f_idx](max_x) - test_derivatives[f_idx](min_x)) / 12;

                    if (!compare_doubles(analytical, conc_out, delta))
                    {
                        printf("[main] Erro %s,\n", case_string);
                        puts("[main] na comparacao com o resultado \"analitico\":");
                        printf("[main] Saida concorrente: %f\n", conc_out);
                        printf("[main] Resultado \"analitico\" + maior termo do erro: %f\n", analytical);
                        printf("[main] (Diferença absoluta: %f)\n", fabs(seq_out - conc_out));

                        puts("");

                    }
                    
                }
            }
        }
    }

    puts("[main] Encerrando execucao com sucesso");

    return EXIT_SUCCESS;
}

void* integrate_subinterval(void* arg)
/* Tarefa executada pelas threads concorrentes. */
{
    thread_args args = *((thread_args*) arg);

    // "Unpacking" os argumentos:

    int id = args.thread_id;
    int curr_n_thr = args.n_threads;
    int f_id = args.function_id;
    int curr_n_subs = args.n_subintervals;
    
    double h = (max_x - min_x) / curr_n_subs;

    int chunk_size = curr_n_subs / curr_n_thr;
    // subintervalos por thread

    int max_iter = chunk_size;  // número de iterações

    if (id == curr_n_thr - 1)
    {
        max_iter += curr_n_subs % curr_n_thr;
        // se for a última thread, seguir até o fim
    }

    double* output;  // para retornar
    output = (double*) safe_malloc(sizeof(double));
    *output = 0;  // acumulador

    double min_xk, max_xk;
    // extremos do intervalo em cada iteração
    min_xk = min_x + id * h * chunk_size;
    double min_f, max_f;
    // valores da função nos extremos
    min_f = test_functions[f_id](min_xk);

    // Aplicando a fórmula do método do trapézio a
    // cada subintervalo que compete à thread:

    for (int k = 0; k < max_iter; k++)
    {
        max_xk = min_xk + h;
        max_f = test_functions[f_id](max_xk);

        *output += min_f + max_f;

        min_xk = max_xk;
        min_f = max_f;
    }

    *output *= h / 2;
    // fazendo isso por último para diminuir a quantidade de
    // operações, e, com isso, o erro numérico (coloquei o
    // fator comum em evidência)

    free(arg);
    // (percebi que tinha que liberar aqui dentro, ou corria
    // o risco de a main liberar antes de a thread terminar de
    // usar hihi)

    pthread_exit((void*) output);
}