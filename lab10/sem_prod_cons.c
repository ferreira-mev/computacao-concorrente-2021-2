/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 10 -- Atividade 1

Problema produtor/consumidor com uso de semáforos.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include "logging.h"
#include "helperfunctions.h"

#define BUFFER_SIZE 5
#define CONS_ITERS 5  // duas escolhas arbitrárias
#define DEBUG

// Variáveis globais:

int should_print;  // imprimir mensagens de log também em stdin

#ifdef DEBUG
should_print = 1;
#else
should_print = 0;
#endif

/* O GCC me dá o seguinte aviso; acredito que ele tenha sido 
"confundido" pelos macros, porque a mensagem não procede:

sem_prod_cons.c:25:1: warning: data definition has no type or storage class
   25 | should_print = 1;
      | ^~~~~~~~~~~~
sem_prod_cons.c:25:1: warning: type defaults to ‘int’ in declaration of ‘should_print’ [-Wimplicit-int]
*/

int buffer[BUFFER_SIZE] = {0};
/* Para facilitar o acompanhamento/o debugging, estou convencionando 
que o buffer é binário: 0 indica uma posição vazia e 1, uma 
preenchida. Entretanto, não estou usando esses valores para controle
da leitura e escrita, dado que, num contexto real, a identificação de
uma posição escrita ou vazia/inválida poderia não ser tão trivial. */

int read_pos = 0;  // índice de onde o consumidor lê
/* Não há um índice para o produtor porque, nesta variante do problema,
ele sempre preenche todas as posições.*/

int n_cons, n_prod;
int cons_iter, prod_iter;
// número de vezes que cada thread lê ou escreve

int running_cons = 0;
/* Se a qtd de posições escritas for menor que a de posições lidas,
alguma(s) thread(s) consumidora(s) ficará(ão) indefinidamente em 
starvation, aguardando que o buffer seja preenchido após todos as 
produtoras terem se encerrado. Se for maior, ocorrerá o oposto, e 
alguma(s) produtora(s) ficará(ão) presa(s) na espera do esvaziamento
do buffer.

Para evitar a necessidade de que o produto do número de iterações 
por consumidor pelo número de consumidores seja sempre múltiplo
do tamanho do buffer vezes o número de produtores (para assegurar o término da execução ajustando o número de iterações por produtor), ou de interromper manualmente a execução do programa, incluí no 
programa uma contagem regressiva de threads leitoras, para garantir
que todas as produtoras se encerrem na ausência de consumidoras. */

sem_t mutex_cons;  // binário, p/ impedir que duas threads leiam a
// mesma pos ao mesmo tempo
sem_t buffer_filled;  // contador de posições preenchidas
sem_t buffer_empty;  // binário; a negação lógica de buffer_filled

char* filename;  // logs
FILE* logfile;

// Cabeçalhos de funções:

void* cons_task(void* cons_args);
void* prod_task(void* prod_args);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        puts("Modo de uso:\n");
        printf("%s n_cons n_prod\n\n", argv[0]);
        puts("onde n_cons é o número de threads consumidoras e n_prod,");
        puts("o de threads produtoras");

        return EXIT_FAILURE;
    }
    
    n_cons = atoi(argv[1]);
    n_prod = atoi(argv[2]);

    int thread_id = -1;  // "ID" da thread main, p/ logs

    filename = (char*) safe_malloc(sizeof(char) * 100);
    sprintf(filename, "logs/");
    strcat(filename, logname(n_cons, n_prod));
    logfile = fopen(filename, "w");

    if (!logfile)
    {
        fprintf(stderr, "Falha na geracao dos arquivos de log\n");
        exit(EXIT_FAILURE); 

    }

    printlog(logfile, thread_id, should_print, "Iniciando execucao");

    // Inicializando vars associadas ao controle de fluxo:
    sem_init(&mutex_cons, 0, 1);  // acesso ao buffer começa liberado
    sem_init(&buffer_filled, 0, 0);  // buffer começa vazio
    sem_init(&buffer_empty, 0, 1);

    // Inicializando threads:

    pthread_t* tid_cons;
    tid_cons = (pthread_t*) safe_malloc(sizeof(pthread_t) * n_cons);

    pthread_t* tid_prod;
    tid_prod = (pthread_t*) safe_malloc(sizeof(pthread_t) * n_prod);

    int* id_range = (int*) safe_malloc(sizeof(int) * (n_cons + n_prod));

    for (int i=0; i < (n_cons + n_prod); i++)
    {
        id_range[i] = i;

        if (i < n_cons)  // thread consumidora
        {
            if (pthread_create(tid_cons + i, NULL, cons_task, (void*) (id_range + i)))
            {
                printlog(logfile, thread_id, 0, "Falha na criacao das threads");
                // should_print = 0 p/ mostrar em stderr em vez
                // de stdin
                fprintf(stderr, "Falha na criacao das threads");
                return EXIT_FAILURE;
            }

            running_cons++;
        }
        else  // thread produtora
        {
            if (pthread_create(tid_prod + (i - n_cons), NULL, prod_task, (void*) (id_range + i)))
            {
                printlog(logfile, thread_id, 0, "Falha na criacao das threads");
                fprintf(stderr, "Falha na criacao das threads");
                return EXIT_FAILURE;
            }
        }
    }

    // Esperando as threads encerrarem:

    for (int i=0; i < n_cons; i++)
    {
        if (pthread_join(*(tid_cons + i), NULL))
        {
            printlog(logfile, thread_id, 0, "Falha na sincronizacao das threads");
            fprintf(stderr, "Falha na sincronizacao das threads");
            return EXIT_FAILURE;
        }   
    }

    for (int i=0; i < n_prod; i++)
    {
        if (pthread_join(*(tid_prod + i), NULL))
        {
            printlog(logfile, thread_id, 0, "Falha na sincronizacao das threads");
            fprintf(stderr, "Falha na sincronizacao das threads");
            return EXIT_FAILURE;
        }   
    }
    
    printlog(logfile, thread_id, should_print, "Encerrando execucao com sucesso");

    printf("Log da execucao salvo em \"%s\".\n", filename);

    // Faxina:

    fclose(logfile);
    free(filename);
    free(tid_cons);
    free(tid_prod);

    sem_close(&mutex_cons);
    sem_close(&buffer_filled);
    sem_close(&buffer_empty);

    return EXIT_SUCCESS;
}


// Definições de funções:

void* cons_task(void* cons_args)
{
    int thread_id = *((int*) cons_args);
    printlog(logfile, thread_id, should_print, "Thread criada");

    for (int iter=0; iter < CONS_ITERS; iter++)
    {
        printlog(logfile, thread_id, should_print, "Aguardando liberacao para leitura (mutex)");
        sem_wait(&mutex_cons);
        printlog(logfile, thread_id, should_print, "Acesso ao buffer liberado (mutex); verificando se esta vazio");
        // se estiver vazio, fica presa aqui; não é problema porque
        // o produtor não depende do mutex, já que, se o buffer estiver
        // vazio, o consumidor já não pode estar lendo, e, como 
        // buffer_empty é binário, dois produtores não podem ser
        // liberados para preencher o buffer ao mesmo tempo
        sem_wait(&buffer_filled);
        printlog(logfile, thread_id, should_print, "Buffer não vazio; lendo...");

        buffer[read_pos] = 0;  // lendo/consumindo

        read_pos = (read_pos + 1) % BUFFER_SIZE;

        if (!read_pos)  // voltou ao começo
        {
            printlog(logfile, thread_id, should_print, "O buffer se esvaziou!");
            sem_post(&buffer_empty);
        }

        printlog(logfile, thread_id, should_print, "Liberando acesso ao buffer (mutex)");
        sem_post(&mutex_cons);
    }

    printlog(logfile, thread_id, should_print, "Thread encerrada");
    running_cons--;  // uma thread consumidora a menos

    if (!running_cons)
    {
        // Se acabaram as threads consumidoras, libera quaisquer
        // produtoras que possam estar aguardando o esvaziamento
        // do buffer

        for (int i=0; i < n_prod; i++)
        {
            sem_post(&buffer_empty);
        }

    }

    pthread_exit(NULL);
}

void* prod_task(void* prod_args)
{
    int thread_id = *((int*) prod_args);
    printlog(logfile, thread_id, should_print, "Thread criada");

    // for (int iter=0; iter < prod_iter; iter++)
    while (running_cons)
    {
        printlog(logfile, thread_id, should_print, "Aguardando buffer vazio");
        sem_wait(&buffer_empty);

        if (!running_cons) { break; }  // vide comentário em cons_task

        printlog(logfile, thread_id, should_print, "Buffer vazio! Preenchendo...");

        for (int i=0; i < prod_iter; i++)
        {
            buffer[i] = 1;  // preenchendo o buffer
        }

        printlog(logfile, thread_id, should_print, "Buffer preenchido; liberando threads consumidoras");

        for (int i=0; i < BUFFER_SIZE; i++)
        {
            sem_post(&buffer_filled);
            // loop separado para que os consumidores realmente só
            // comecem a ler após todo o buffer ter sido preenchido
            // "de uma vez"
        }

    }

    printlog(logfile, thread_id, should_print, "Sem mais consumidores; thread encerrada");
    pthread_exit(NULL);
}