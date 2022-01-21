/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções auxiliares, diferentes das funções matemáticas de teste e dos
fluxos de execução das threads principal e paralelas.
*/

#include <stdio.h>
#include <stdlib.h>

#include "helperfunctions.h"

void* safe_malloc(size_t size)
/* "Wrap" para a função malloc com verificação de falha na alocação. */
{
   void* ptr = malloc(size);

   if (!ptr)
   {
      fprintf(stderr, "Falha na alocacao de memoria\n");
      exit(EXIT_FAILURE);
   }

   return ptr;
}