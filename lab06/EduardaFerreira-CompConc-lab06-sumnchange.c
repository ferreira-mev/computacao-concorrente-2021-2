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

// Variáveis globais:

int NTHR;

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

int main(int argc, char *argv[])
{
    puts("Iniciando execucao");

    if (argc != 2)
    {
        puts("Modo de uso:\n");
        printf("%s N\n\n", argv[0]);
        puts("onde N e o numero de threads");

        return EXIT_FAILURE;

    }
    NTHR = atoi(argv[1]);

    printf("Numero de threads fornecido: %d\n", NTHR);
    

    puts("Encerrando execucao com sucesso");
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