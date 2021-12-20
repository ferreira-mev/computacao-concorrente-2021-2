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

int nprints = 0;
pthread_mutex_t mutex_nprints;
pthread_cond_t cond_mid;  // thread 0 libera 1, 2, 3
pthread_cond_t cond_end;  // última de 1, 2, 3 libera 4


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

    // Inicializando vars associadas ao controle de fluxo:
    pthread_mutex_init(&mutex_nprints, NULL);
    pthread_cond_init(&cond_mid, NULL);
    pthread_cond_init(&cond_end, NULL);

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

    // Apagando a luz e fechando a porta:
    free(id_range);
    pthread_mutex_destroy(&mutex_nprints);
    pthread_cond_destroy(&cond_mid);
    pthread_cond_destroy(&cond_end);
    
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
    printf("\nBeginning execution of thread %d; nprints = %d\n", id, nprints);
    #endif

    pthread_mutex_lock(&mutex_nprints);

    switch (id)
    {
        case 0:
            while (nprints < 4)
            {
                #ifdef DEBUG
                printf("\nThread %d will block itself; nprints = %d\n", id, nprints);
                #endif

                pthread_cond_wait(&cond_end, &mutex_nprints);

                #ifdef DEBUG
                printf("\nThread %d received an unlocking signal; nprints = %d\n", id, nprints);
                #endif
            }

            printf("%s\n", sentences[id]);
            nprints++;  // só por consistência, não muda nada :P

            break;
        case 1:
        case 2:
        case 3:
            while (!nprints)
            {
                #ifdef DEBUG
                printf("\nThread %d will block itself; nprints = %d\n", id, nprints);
                #endif

                pthread_cond_wait(&cond_mid, &mutex_nprints);

                #ifdef DEBUG
                printf("\nThread %d received an unlocking signal; nprints = %d\n", id, nprints);
                #endif
            }

            printf("%s\n", sentences[id]);
            nprints++;

            if (nprints == 4)
            {
                pthread_cond_signal(&cond_end);

                // Basta signal em vez de broadcast pois só pode haver 
                // uma thread esperando (a 4)
            }

            break;
        case 4:
            printf("%s\n", sentences[id]);
            nprints++;

            pthread_cond_broadcast(&cond_mid);
            break;
        default:
            fprintf(stderr, "Erro - estado invalido");
            exit(EXIT_FAILURE);
    }

    pthread_mutex_unlock(&mutex_nprints);

    #ifdef DEBUG
    printf("\nEnding execution of thread %d; nprints = %d\n", id, nprints);
    #endif
    
    pthread_exit(NULL);
}