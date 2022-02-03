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

int n_cons, n_prod;

char* filename;  // logs
FILE* logfile;

// definir um buffer aqui

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
    int thread_id = -1;  // "ID" da thread main

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

    pthread_t* tid_cons;
    tid_cons = (pthread_t*) safe_malloc(sizeof(pthread_t) * n_cons);

    pthread_t* tid_prod;
    tid_prod = (pthread_t*) safe_malloc(sizeof(pthread_t) * n_prod);

    int* id_range = (int*) safe_malloc(sizeof(int) * (n_cons + n_prod));

    // inicializar vars associadas ao controle de fluxo

    printlog(logfile, thread_id, should_print, "Inicializando threads");

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

    // lembrar do sem_close

    return EXIT_SUCCESS;
}


// Definições de funções:

void* cons_task(void* cons_args)
{
    int thread_id = *((int*) cons_args);
    printlog(logfile, thread_id, should_print, "Thread criada");

    printlog(logfile, thread_id, should_print, "Thread encerrada");
    pthread_exit(NULL);
}

void* prod_task(void* prod_args)
{
    int thread_id = *((int*) prod_args);
    printlog(logfile, thread_id, should_print, "Thread criada");

    printlog(logfile, thread_id, should_print, "Thread encerrada");
    pthread_exit(NULL);
}