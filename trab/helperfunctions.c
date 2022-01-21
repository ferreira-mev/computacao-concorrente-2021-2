/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções auxiliares, diferentes das funções matemáticas de teste e dos
fluxos de execução das threads principal e paralelas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

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

short int compare_doubles(double x, double y, double delta)
/* Retorna 0 se os números de ponto flutuante x e y podem ser
considerados iguais por uma margem delta, e 1 caso contrário.

Adaptada de https://floating-point-gui.de/errors/comparison/ */
{
    double abs_x = fabs(x);
    double abs_y = fabs(y);
    double abs_diff = fabs(x - y);

    if (x == y)
    {
        return 0;
    }
    else if (!x || !y || (abs_x + abs_y < DBL_MIN))
    {
        return abs_diff < (delta * DBL_MIN);
    }
    else
    {
        return abs_diff / (abs_x + abs_y) < delta;
    }
}