/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 5 -- Atividade 4
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5
#define MAX_STRLEN 25  // (com folga)

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

int main(int argc, char *argv[])
{
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