/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 1 -- Atividade 5

Programa que eleva ao quadrado os elementos de um vetor, alterando o vetor inicial, com o uso de duas threads paralelas além do programa principal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2  // número de threads (além do main)
#define NELEMS 5 //10000  // número de entradas do vetor

#define DEBUG

typedef struct
{
    int thr_n;  // número da thread
    int* vector;  // o array, em sua totalidade
    // (ponteiro p/ sua posição inicial)
} thr_args;

void* square_elems(void* arg)
/* Recebe um ponteiro para uma struct do tipo thr_args e eleva ao quadrado parte das entradas do vetor apontado por seu campo vector,
de acordo com o número da thread thr_n. */
{
    thr_args* args = (thr_args*) arg;

    // Quantidade de elementos pelos quais a thread é responsável:
    int nelems = (NELEMS / NTHREADS) - 1;

    if ((NELEMS % NTHREADS) > args->thr_n)
    {
        nelems++;
    }

    for (int j=0; j < nelems; j++)
    {
        int pos = j * NTHREADS + args->thr_n;

        #ifdef DEBUG
            printf("Thread %d squaring position %d: ", args->thr_n, pos);
            printf("%d**2 = ", (args->vector)[pos]);
        #endif

        (args->vector)[pos] *= (args->vector)[pos];

        #ifdef DEBUG
            printf("%d\n", (args->vector)[pos]);
        #endif
        
    }

    free(arg);

    pthread_exit(NULL);
}

int main()
{

    #ifdef DEBUG
        printf("Beginning execution\n");
    #endif

    int input_vector[5] = {4, 6, 2, 9, 0};

    #ifdef DEBUG
        printf("Vector to square: \n");

        for (int i=0; i < NELEMS; i++)
        {
            printf("%d ", input_vector[i]);
        }

        printf("\n");
    #endif

    pthread_t tid_sistema[NTHREADS];

    thr_args* arg;

    // Criando as threads:
    for (int thread=0; thread<NTHREADS; thread++)
    {
        arg = malloc(sizeof(thr_args));

        if (arg == NULL)
        {
            printf("Falha na alocacao\n");
            exit(1);
        }

        arg->thr_n = thread;
        arg->vector = input_vector;

        #ifdef DEBUG
            printf("Creating thread %d\n", thread);
        #endif

        if (pthread_create(&tid_sistema[thread], NULL, square_elems, (void*) arg))
        {
            printf("Erro na criacao da thread %d\n", thread);
        }
    }

    // Aguardando todas as threads:
    for (int thread=0; thread<NTHREADS; thread++)
    {
        if (pthread_join(tid_sistema[thread], NULL))
        {
            printf("Erro ao aguardar a thread %d\n", thread);
        }
    }

    // Imprimindo o resultado:
    for (int i=0; i < NELEMS; i++)
    {
        printf("%d ", input_vector[i]);
    }

    printf("\n");

    #ifdef DEBUG
        printf("Ending execution\n");
    #endif
}