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

int n_threads[6] = { 1, 2, 4, 6, 8, 10 };
int n_subintervals[3] = { 10, 100, 1000 };
int n_runs = 5;

typedef struct
{
    int thread_id;
    int function_id;
    int n_subintervals;
} thread_args;

typedef double (*testfoo_ptr)(double);

double test_f1(double x);
double test_f2(double x);
double test_f3(double x);

double primitive_f1(double x);
double primitive_f2(double x);
double primitive_f3(double x);

testfoo_ptr test_functions[3] =
{
    test_f1, test_f2, test_f3
};

testfoo_ptr test_primitives[3] =
{
    primitive_f1, primitive_f2, primitive_f3
};

double min_x = 1.0;
double max_x = 11.0;  // integração em [1, 11]

void* safe_malloc(size_t size);
void* integrate_subinterval(void* arg);


// Fluxo da thread principal:

int main(int argc, char *argv[])
{
    puts("[main] Iniciando execucao");

    // printf("[main] Testando: f1(0) = %f\n", test_functions[2](0));

    // printf("[main] Testando: F2(1) = %f\n", test_primitives[1](1));

    pthread_t* tid;
    double seq_out;
    double* thread_out;
    double conc_out;

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
                }
            }
        }
    }

    puts("[main] Encerrando execucao com sucesso");

    return EXIT_SUCCESS;
}

// Definições de funções:

double test_f1(double x)  // 4x^5 + x^2 - 3x - 5
{
    return 4*pow(x, 5) + pow(x, 2) - 3*x + 5;
}

double test_f2(double x)
{
    return 1 / x;
}

double test_f3(double x)
{
    return cos(2*x);
}

double primitive_f1(double x)  // 2/3 x^6 + x^3 / 3 - 3/2 x^2 - 5x
{
    return 2/3 * pow(x, 6) + pow(x, 3) / 3 - 3/2 * pow(x, 2) - 5*x;
}

double primitive_f2(double x)
{
    return log(x);
}

double primitive_f3(double x)
{
    return sin(x) * cos(x);
}

void* safe_malloc(size_t size)
/* "Wrap" para a função malloc com verificação de falha na alocação. */
{
   void* ptr = malloc(size);

   if (!ptr)
   {
      fprintf(stderr, "Falha na alocacao de memoria\n");
      exit(EXIT_FAILURE);
   }

   return ptr;
}

void* integrate_subinterval(void* arg)
/* Tarefa executada pelas threads concorrentes. */
{
    double* output;

    output = (double*) safe_malloc(sizeof(double));

    *output = 0;

    pthread_exit((void*) output);
}