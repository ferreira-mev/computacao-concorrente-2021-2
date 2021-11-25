/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 2 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

// Variáveis globais:
int dim;
int nthreads;

// Cabeçalhos de funções:

void seq_mat_mul(int dim, int* mat1, int* mat2, int* out);

void display_matrix(int dim, int* mat);

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   if (argc < 2) { dim = 4; } // temporário, para teste
   else
   { 
      dim = atoi(argv[1]);
      nthreads = atoi(argv[2]);
   }

   // Alocando memória:
   int* mat1 = malloc(sizeof(int) * dim * dim);
   int* mat2 = malloc(sizeof(int) * dim * dim);
   int* out = malloc(sizeof(int) * dim * dim);

   if (!(mat1 && mat2 && out))
   {
      printf("Falha na alocação");
      return EXIT_FAILURE;
   }

   // Preenchendo deterministicamente para teste:
   for (int i=0; i < dim; i++)
   {
      for (int j=0; j < dim; j++)
      {
         mat1[i * dim + j] = i + j;
         mat2[i * dim + j] = j;
         out[i * dim + j] = 0;  // acumulador
      }
   }

   display_matrix(dim, mat1);
   display_matrix(dim, mat2);

   // Testando multiplicação sequencial:
   seq_mat_mul(dim, mat1, mat2, out);
   display_matrix(dim, out);

   // Liberando a memória alocada:
   free(mat1);
   free(mat2);
   free(out);

   return EXIT_SUCCESS;
}

// Definições de funções:

void seq_mat_mul(int dim, int* mat1, int* mat2, int* out)
/* Função para multiplicação sequencial de matrizes quadradas
dim x dim.

A matriz de saída deve ter sido previamente alocada e fornecida
por referência, por meio do parâmetro out.
*/
{
   for (int i=0; i < dim; i++)
   {
      for (int j=0; j < dim; j++)
      {
         for (int k=0; k < dim; k++)
         {
            out[i * dim + j] += mat1[i * dim + k] * mat2[k * dim + j];
         }
      }
   }
}

void display_matrix(int dim, int* mat)
/* Imprime matriz quadrada dim x dim. */
{
   for (int i=0; i < dim; i++)
   {
      printf("[");
      for (int j=0; j < dim; j++)
      {
         printf("%d", mat[i * dim + j]);

         if (j < dim-1) { printf(", "); }
      }
      printf("]\n");
   }

   puts("");
}