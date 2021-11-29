/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 2 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
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

int verify_conc_soln(int* seq_out, int* conc_out);

/* Obs: Sei que não seria necessário passar dim para as funções por se
tratar de uma variável global; dim aparece como parâmetro por um
"artefato histórico", por assim dizer. Eu segui a sequência sugerida 
nas instruções do laboratório e comecei fazendo um programa sequencial,
para servir como teste para a saída concorrente, então, de início, não
trabalhei com variáveis globais. Quando introduzi a versão concorrente, 
e com ela as variáveis globais, acabei "não mexendo" nas funções que
já havia escrito anteriormente. Ao me dar conta disso, achei que não
valia o refactor e a chance de introduzir um bug; deixei funcionando
como estava. */

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   pthread_t* tid;
   double t0, tf, dt;

   int NRUNS;
   int NDIMS;
   int NNTHR;

   int dim_arr[NDIMS];
   int nthreads_arr[NNTHR];

   double t_seq[NDIMS * NNTHR];
   double t_conc[NDIMS * NNTHR];
   // Linhas: dimensão
   // Colunas: número de threads
   
   if (argc == 1)
   { 
      puts("Programa executado sem passagem de argumentos");
      puts("pela linha de comando; executando em modo de teste");
      puts("de desempenho");

      NRUNS = 5;
      NDIMS = 3;
      NNTHR = 4;
      // (o C não permite inicializar arrays de tamanho
      // dado por variável -- não posso, por exemplo, usar
      // int dim_arr[NDIMS] = {500, 1000, 200};)

      int n0 = 500;

      for (int d=0; d < NDIMS; d++)
      {
         dim_arr[d] = n0;
         n0 *= 2;

         #ifdef DEBUG
         printf("dim_arr[%d] = %d\n", d, dim_arr[d]);
         #endif
      }

      n0 = 1;

      for (int n=0; n < NNTHR; n++)
      {
         nthreads_arr[n] = n0;
         n0 *= 2;

         #ifdef DEBUG
         printf("nthreads_arr[%d] = %d\n", n, nthreads_arr[n]);
         #endif
      }

      // dim_arr = {500, 1000, 2000};
      // nthreads_arr = {1, 2, 4, 8};

      
   }
   else if (argc == 3)
   { 
      dim = atoi(argv[1]);
      nthreads = atoi(argv[2]);

      if (nthreads > dim)
      {
         printf("O numero de threads fornecido (%d) e maior que\n", nthreads);
         printf("numero de linhas (%d) e sera reduzido para %d\n\n", dim, dim);

         nthreads = dim;
         // (Eu podia só usar um min entre dim e nthreads, mas eu queria
         // o if para avisar ao usuário)
      }

      int dim_arr[1] = {dim};
      int nthreads_arr[1] = {nthreads};

      NRUNS = 1;
      NDIMS = 1;
      NNTHR = 1;

      #ifdef DEBUG
      printf("dim = %d, nthreads = %d\n\n", dim, nthreads);
      #endif
   }
   else
   {
      puts("Modo de uso:\n");

      puts("Para multiplicacao de matrizes quadradas dim x dim");
      puts("com entradas aleatorias, usando n threads:");
      printf("%s dim n\n\n", argv[0]);

      puts("Para avaliacao de desempenho:");
      printf("%s\n", argv[0]);

      return EXIT_FAILURE;
   }

   srand(time(NULL));

   for (int d=0; d < NDIMS; d++)
   {
      for (int n=0; n < NNTHR; n++)
      {
         dim = dim_arr[d];
         nthreads = nthreads_arr[n];
      }
   }

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
         mat1[i * dim + j] = i * dim + j;
         mat2[i * dim + j] = 1;
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

   // display_matrix(dim, mat1);
   // display_matrix(dim, mat2);
   #endif

   // Criação das threads:

   GET_TIME(t0);  // início da medição concorrente

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

   GET_TIME(tf);  // fim da medição concorrente
   
   dt = tf - t0;

   #ifdef DEBUG
   printf("Duracao da etapa concorrente: %lf segundos\n", dt);
   #endif

   // Comparando com a multiplicação sequencial:
   GET_TIME(t0);  // início da medição sequencial

   seq_mat_mul(dim, mat1, mat2, seq_out);

   GET_TIME(tf);  // fim da medição sequencial

   dt = tf - t0;

   #ifdef DEBUG
   printf("Duracao da etapa sequencial: %lf segundos\n", dt);
   #endif
   
   #ifndef DEBUG
   if (verify_conc_soln(seq_out, conc_out)) { return EXIT_FAILURE; }

   #else
   puts("Resultado sequencial:\n");
   // display_matrix(dim, seq_out);

   // puts("\nResultado concorrente:\n");
   // display_matrix(dim, conc_out);
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
            conc_out[row * dim + j] += mat1[row * dim + k] * mat2[k * dim + j];
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

int verify_conc_soln(int* seq_out, int* conc_out)
/* Verifica a solução concorrente por meio da comparação com
a saída da função sequencial. */
{
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

            return 1;
         }
      }
   }

   return 0;
}