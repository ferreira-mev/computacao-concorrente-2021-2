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

    printf("[main] Testando: f1(0) = %f\n", test_functions[2](0));

    printf("[main] Testando: F2(1) = %f\n", test_primitives[1](1));

    pthread_t* tid;
    double seq_out;
    double* thread_out;
    double conc_out;

    double delta = pow(10, -6);  // p/ comparar seq e conc

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
                        curr_args->function_id = f_idx;
                        curr_args->n_subintervals = n_subintervals[n_idx];

                        if (pthread_create(tid + t, NULL, integrate_subinterval, (void*) curr_args))
                        {
                            fprintf(stderr, "Falha na criacao das threads");
                            return EXIT_FAILURE;
                        }

                        free(curr_args);
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

                    // Comparando resultados sequencial e concorrente:

                    // (remember: fabs, not abs, for double)

                    // https://floating-point-gui.de/errors/comparison/
                }
            }
        }
    }

    puts("[main] Encerrando execucao com sucesso");

    return EXIT_SUCCESS;
}

// Definições de funções:

void* integrate_subinterval(void* arg)
/* Tarefa executada pelas threads concorrentes. */
{
    double* output;

    output = (double*) safe_malloc(sizeof(double));

    *output = 0;

    pthread_exit((void*) output);
}