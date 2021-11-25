/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 2 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define DEBUG

// Variáveis globais:

int dim;
int nthreads;

int* mat1;
int* mat2;

int* conc_out;

// Cabeçalhos de funções:

void seq_mat_mul(int dim, int* mat1, int* mat2, int* seq_out);

void display_matrix(int dim, int* mat);

void* conc_mat_mul(void* arg);

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   pthread_t* tid;
   
   if (argc == 1)
   { 
      #ifdef DEBUG
      dim = 4; nthreads = 1;
      #else
      puts("Programa executado sem passagem de argumentos");
      puts("pela linha de comando; executando em modo de teste");
      puts(" de desempenho");
      #endif
      }
   else if (argc == 3)
   { 
      dim = atoi(argv[1]);
      nthreads = atoi(argv[2]);
   }
   else
   {
      puts("Modo de uso:\n");

      puts("Para multiplicacao de matrizes quadradas dim x dim");
      puts(" com entradas aleatorias, usando n threads:");
      printf("%s dim n\n\n", argv[0]);

      puts("Para avaliacao de desempenho:");
      printf("%s\n", argv[0]);

      return EXIT_FAILURE;
   }

   srand(time(NULL));

   // Alocando memória:
   mat1 = malloc(sizeof(int) * dim * dim);
   mat2 = malloc(sizeof(int) * dim * dim);

   int* seq_out = malloc(sizeof(int) * dim * dim);
   conc_out = malloc(sizeof(int) * dim * dim);

   int* nrange = malloc(sizeof(int) * nthreads);

   if (!(mat1 && mat2 && seq_out && conc_out && nrange))
   {
      puts("Falha na alocação de memória");
      return EXIT_FAILURE;
   }

   // Preenchendo nrange com os identificadores "internos"
   // das threads:
   for (int t=0; t < nthreads; t++)
   {
      nrange[t] = t;
   }

   // Preenchendo matrizes de entrada com inteiros aleatórios:

   for (int i=0; i < dim; i++)
   {
      for (int j=0; j < dim; j++)
      {
         #ifdef DEBUG
         mat1[i * dim + j] = i + j;
         mat2[i * dim + j] = i;
         #else
         mat1[i * dim + j] = rand();
         mat2[i * dim + j] = rand();
         #endif

         seq_out[i * dim + j] = 0;  // acumulador
         conc_out[i * dim + j] = 0;  // idem
      }
   }

   #ifdef DEBUG
   puts("Matrizes de entrada:\n");

   display_matrix(dim, mat1);
   display_matrix(dim, mat2);
   #endif

   // Criação das threads:

   tid = (pthread_t*) malloc(sizeof(pthread_t*) * nthreads);
   
   if (!tid)
   {
      puts("Falha na alocação de memória");
      return EXIT_FAILURE;
   }

   for (int t=0; t < nthreads; t++)
   {
      if (pthread_create(&(tid[t]), NULL, conc_mat_mul, (void*) (nrange+t)))
      {
         puts("Falha na criação das threads");
         return EXIT_FAILURE;
      }
   }

   // Aguardando a execução de todas as threads:

   for (int t=0; t < nthreads; t++)
   {
      pthread_join(tid[t], NULL);
   }

   // Comparando com amultiplicação sequencial:
   seq_mat_mul(dim, mat1, mat2, seq_out);
   
   #ifndef DEBUG
   for (int i=0; i < dim; i++)
   {
      for (int j=0; j < dim; j++)
      {
         int idx = i * dim + j;

         if (seq_out[idx] != conc_out[idx])
         {
            printf("Falha no programa concorrente: ");
            printf("resultado diferente da multiplicacao sequencial\n");

            printf("\nPrimeira divergencia: linha %d, coluna %d\n", i, j);

            printf("Resultado sequencial: %d\n", seq_out[idx]);
            printf("Resultado concorrente: %d\n", conc_out[idx]);

            return EXIT_FAILURE;
         }
      }
   }

   #else
   puts("Resultado sequencial:\n");
   display_matrix(dim, seq_out);

   puts("\nResultado concorrente:\n");
   display_matrix(dim, conc_out);
   #endif

   // Liberando a memória alocada:
   free(mat1);
   free(mat2);

   free(seq_out);
   free(conc_out);

   free(nrange);

   return EXIT_SUCCESS;
}

// Definições de funções:

void seq_mat_mul(int dim, int* mat1, int* mat2, int* seq_out)
/* Função para multiplicação sequencial de matrizes quadradas
dim x dim.

A matriz de saída deve ter sido previamente alocada e fornecida
por referência, por meio do parâmetro seq_out.
*/
{
   for (int i=0; i < dim; i++)
   {
      for (int j=0; j < dim; j++)
      {
         for (int k=0; k < dim; k++)
         {
            seq_out[i * dim + j] += mat1[i * dim + k] * mat2[k * dim + j];
         }
      }
   }
}

void* conc_mat_mul(void* arg)
/* Tarefa passada a cada thread do programa concorrente para
multiplicação de matrizes dim x dim.

A thread t fica responsável pelo cálculo das linhas de índice t, t + 
nthreads, ... t + k * nthreads, para k natural, enquanto t + 
k * nthreads < dim.
*/
{
   int id = *((int*) arg);

   int row = id;
   int counter = 0;

   do
   {
      for (int j=0; j < dim; j++)
      {
         for (int k=0; k < dim; k++)
         {
            conc_out[row + j] += mat1[row + k] * mat2[k * dim + j];
         }
      }
      counter++;
      row = id + nthreads * counter;  // da próx iteração
   } while (row < dim);

   pthread_exit(NULL);
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