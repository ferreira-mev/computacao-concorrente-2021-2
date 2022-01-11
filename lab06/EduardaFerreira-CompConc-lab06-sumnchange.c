/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 6 -- Atividade 1

Obs: Como, desta vez, imprimir strings que indicassem o andamento da 
execução foi incluído como parte explícita da tarefa, tornei a 
aplicação "verbosa por default", em vez de silenciar a maior parte
das saídas fora do modo debug, como nos laboratórios anteriores.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Variáveis globais:

int NTHR;
int* rvec;
int* id_range;

int running_threads;  // "contador reverso" de threads não encerradas
// (numa dada iteração)
pthread_mutex_t mutex_rt;
pthread_cond_t cond_barr;

// Cabeçalhos de funções:

void* safe_malloc(size_t size);
void* sum_and_change(void* p_id);
void barrier();

// Fluxo da thread principal:

int main(int argc, char *argv[])
{
    puts("[main] Iniciando execucao");

    if (argc != 2)
    {
        puts("Modo de uso:\n");
        printf("%s N\n\n", argv[0]);
        puts("onde N e o numero de threads");

        return EXIT_FAILURE;

    }

    NTHR = atoi(argv[1]);
    pthread_t tid[NTHR];
    running_threads = NTHR;

    printf("[main] Numero de threads fornecido: %d\n", NTHR);
    
    printf("[main] Inicializando pseudo-RNG e vetor de inteiros aleatorios\n");
    srand(time(NULL));
    
    rvec = (int*) safe_malloc(sizeof(int) * NTHR);
    id_range = (int*) safe_malloc(sizeof(int) * NTHR);

    for (int i = 0; i < NTHR; i++)
    {
        id_range[i] = i;
        rvec[i] = (rand() % 10);
        // A rigor, não preserva a equiprobabilidade, mas isso não é
        // crucial aqui
        printf("[main] Posição %d: %d\n", i, rvec[i]);
    }

    puts("[main] Criando threads");

    for (int t=0; t < NTHR; t++)
    {
        if (pthread_create(tid + t, NULL, sum_and_change, (void*) (id_range + t)))
        {
            fprintf(stderr, "[main] Falha na criacao das threads");
            return EXIT_FAILURE;
        }
    }

    // Esperando as threads terminarem:
    for (int t=0; t < NTHR; t++)
    {
        if (pthread_join(*(tid + t), NULL))
        {
            fprintf(stderr, "[main] Falha na sincronizacao das threads");
            return EXIT_FAILURE;
        }
    }

    puts("[main] Execucao de todas as threads concluida");

    free(id_range);
    free(rvec);

    puts("[main] Encerrando execucao do programa principal com sucesso");
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

void* sum_and_change(void* p_id)
/* Tarefa de cada thread, conforme descrita nas especificações do 
trabalho. */
{
    int id = *((int*) p_id);
    
    printf("[thread %d] Iniciando execucao\n", id);

    pthread_exit(NULL);
}

void barrier()
/* Função barreira. */
{
    pthread_mutex_lock(&mutex_rt);

    if (!running_threads)
    {
        pthread_cond_broadcast(&cond_barr);
        running_threads = NTHR;
    }
    else
    {
        running_threads--;
        pthread_cond_wait(&cond_barr, &mutex_rt);
    }

    pthread_mutex_ulock(&mutex_rt);

}