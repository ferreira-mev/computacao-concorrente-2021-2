/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 2 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

void seq_mat_mul(int dim, int* mat1, int* mat2, int* out)
/* Função para multiplicação sequencial de matrizes quadradas
dim x dim.

A matriz de saída deve ter sido previamente alocada e fornecida
por referência, por meio do parâmetro out.
*/
{

}

int main(int argc, char* argv[])
{
   int dim = 10;  // temporário, para teste

   // Preenchendo matrizes de uns, para teste:
   int* mat1 = malloc(sizeof(int) * dim * dim);
   int* mat2 = malloc(sizeof(int) * dim * dim);
   int* out = malloc(sizeof(int) * dim * dim);

   if (!(mat1 && mat2 && out))
   {
      printf("Falha na alocação");
      return EXIT_FAILURE;
   }

   // Liberando a memória alocada:
   free(mat1);
   free(mat2);
   free(out);

   return EXIT_SUCCESS;
}
