/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 9 -- Atividade 1

Adaptado do laboratório 5.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS  5
#define MAX_STRLEN 25  // (com folga)
// (desperdiça um pouquinho de memória, mas nesse caso é mesmo pouco, e
// simplifica a inicialização do array sentences abaixo)

// #define DEBUG

// Variáveis globais:

sem_t* conds;
// semáforos "booleanos"/"binários"; a posição i indica se a thread
// i + 2 já imprimiu (considerando a indexação a partir do 1, como nas
// instruções do laboratório; ou i + 1, indexando do zero, como fiz 
// aqui por simplicidade)

char sentences[NTHREADS][MAX_STRLEN] = {
    "Volte sempre!",
    "Fique a vontade.",
    "Sente-se por favor.",
    "Aceita um copo d'agua?",
    "Seja bem-vindo!"
}; // frases a imprimir

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

void* print_sentence(void* p_id);  // tarefa das threads

int main(int argc, char *argv[])
{
    #ifdef DEBUG
    puts("Beginning execution");
    #endif

    pthread_t tid[NTHREADS];
    int* id_range = (int*) safe_malloc(sizeof(int) * NTHREADS);

    // Inicializando vars associadas ao controle de fluxo:
    conds = (sem_t*) safe_malloc(sizeof(sem_t) * (NTHREADS - 1));
    // a última thread não precisa de condição

    for (int i = 3; i >= 0; i--)
    {
        sem_init(conds + i, 0, 0);
    }

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

    for (int i = 3; i >= 0; i--)
    {
        sem_close(conds + i);
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
/* Tarefa a ser executada pelas threads. */
{
    int id = *((int*) p_id);

    #ifdef DEBUG
    printf("\nBeginning execution of thread %d\n", id);
    #endif

    switch (id)
    {
        case 0:
            for (int i = 3; i >= 0; i--)
            // aguardando todas as demais threads
            {
                sem_wait(conds + i);
            }

            printf("%s\n", sentences[id]);
            
            break;
        case 1:
        case 2:
        case 3:
            sem_wait(conds + 3);  // aguardando a thread 5

            printf("%s\n", sentences[id]);

            sem_post(conds + (id - 1));

            break;
        case 4:
            // é a primeira a executar, não precisa aguardar nenhuma condição

            printf("%s\n", sentences[id]);

            for (int i = 0; i < 4; i++)
            // aguardando todas as demais threads
            {
                sem_post(conds + (id - 1));
                // precisa haver um sinal disponível para
                // cada uma das 4 outras threads que espera
                // pela thread 5
            }

            
            break;
        default:
            fprintf(stderr, "Erro - estado invalido");
            exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
    printf("\nEnding execution of thread %d\n", id);
    #endif
    
    pthread_exit(NULL);
}