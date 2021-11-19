/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 1 -- Atividade 5

Programa que eleva ao quadrado os elementos de um vetor, alterando o vetor inicial, com o uso de duas threads paralelas além do programa principal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2  // número de threads (além do main)
#define NELEMS 10000  // número de entradas do vetor

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

    pthread_exit(NULL);
}