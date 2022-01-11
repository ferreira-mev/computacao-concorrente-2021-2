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

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

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

    printf("[main] Numero de threads fornecido: %d\n", NTHR);
    
    printf("[main] Inicializando pseudo-RNG e vetor de inteiros aleatorios\n");
    srand(time(NULL));
    
    rvec = safe_malloc(sizeof(int) * NTHR);

    for (int i = 0; i < NTHR; i++)
    {
        rvec[i] = (rand() % 10);
        // A rigor, não preserva a equiprobabilidade, mas isso não é
        // crucial aqui
        printf("[main] Posição %d: %d\n", i, rvec[i]);
    }

    puts("[main] Encerrando execucao com sucesso");
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