/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 5 -- Atividade 4
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5
#define MAX_STRLEN 25  // (com folga)
#define DEBUG

// Variáveis globais:

int filling = 0;  // "booleano"; está no recheio?
pthread_mutex_t mutex_filling;
pthread_cond_t cond_filling;

char sentences[NTHREADS][MAX_STRLEN] = {
    "Volte sempre!",
    "Fique a vontade.",
    "Sente-se por favor.",
    "Aceita um copo d'agua?",
    "Seja bem-vindo!"
}; // frases a imprimir

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

void* print_sentence(void* p_id);

int main(int argc, char *argv[])
{
    #ifdef DEBUG
    puts("Beginning execution");
    #endif

    pthread_t tid[NTHREADS];

    int* id_range = (int*) safe_malloc(sizeof(int) * NTHREADS);
    // Preenchendo id_range com os identificadores "internos"
    // das threads:
    for (int t=0; t < NTHREADS; t++)
    {
        id_range[t] = t;
    }

    // Criando as threads:
    for (int t=0; t < NTHREADS; t++)
    {
        if (pthread_create(tid + t, NULL, print_sentence, (void*) (id_range + t)))
        {
            fprintf(stderr, "Falha na criacao das threads");
            return EXIT_FAILURE;
        }
    }

    // Esperando as threads terminarem:
    for (int t=0; t < NTHREADS; t++)
    {
        if (pthread_join(*(tid + t), NULL))
        {
            fprintf(stderr, "Falha na sincronizacao das threads");
            return EXIT_FAILURE;
        }
    }

    #ifdef DEBUG
    puts("Exiting successfully");
    #endif

    return EXIT_SUCCESS;
}


// Definições de funções:

void* safe_malloc(size_t size)
/* "Wrap" para a função malloc com verificação de falha na alocação. */
{
   void* ptr = malloc(size);

   if (!ptr)
   {
      fprintf(stderr, "Falha na alocacao de memoria");
      exit(EXIT_FAILURE);
   }

   return ptr;
}

void* print_sentence(void* p_id)
{
    int id = *((int*) p_id);

    #ifdef DEBUG
    printf("Beginning execution of thread %d\n", id);
    #endif

    printf("%s\n", sentences[id]);

    pthread_exit(NULL);
}