/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 2 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

// #define DEBUG

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

int verify_conc_soln(int dim, int* seq_out, int* conc_out);

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   int* seq_out;
   int* nrange;
   pthread_t* tid;
   double t0, tf, dt;

   int nruns;
   int ndims;
   int nnthr;

   int* dim_arr;
   int* nthreads_arr;

   double* t_seq;
   double* t_conc;
   // Linhas: dimensão
   // Colunas: número de threads
   
   if (argc == 1)
   { 
      puts("Programa executado sem passagem de argumentos");
      puts("pela linha de comando; executando em modo de teste");
      puts("de desempenho");

      #ifdef DEBUG
      nruns = 2;
      ndims = 2;
      nnthr = 2;
      #else
      nruns = 5;
      ndims = 3;
      nnthr = 4;
      #endif
   }
   else if (argc == 3)
   { 
      dim = atoi(argv[1]);
      nthreads = atoi(argv[2]);

      #ifdef DEBUG
      printf("dim = %d, nthreads = %d\n\n", dim, nthreads);
      #endif

      if (nthreads > dim)
      {
         printf("O numero de threads fornecido (%d) e maior que\n", nthreads);
         printf("numero de linhas (%d) e sera reduzido para %d\n\n", dim, dim);

         nthreads = dim;
         // (Eu podia só usar um min entre dim e nthreads, mas eu queria
         // o if para avisar ao usuário)
      }

      nruns = 1;
      ndims = 1;
      nnthr = 1;
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

   dim_arr = malloc(sizeof(int) * ndims);
   nthreads_arr = malloc(sizeof(int) * nnthr);

   t_seq = malloc(sizeof(double) * ndims * nnthr);
   t_conc = malloc(sizeof(double) * ndims * nnthr);

   if (!(dim_arr && nthreads_arr && t_seq && t_conc))
   {
      puts("Falha na alocacao de memoria");
      return EXIT_FAILURE;
   }

   if (argc == 1)
   { 
      /* Contorno ad hoc e meio gambiarrado para a impossibilidade 
      de inicialização de arrays de tamanho variável em C (eu queria
      seguir o mesmo fluxo da main para argc = 1 ou 3); construindo
      
      dim_arr = {500, 1000, 2000}
      nthreads_arr = {1, 2, 4, 8}
      */

      int n0 = 500;

      for (int d=0; d < ndims; d++)
      {
         dim_arr[d] = n0;
         n0 *= 2;

         #ifdef DEBUG
         // dim_arr[d] = 5;
         printf("dim_arr[%d] = %d\n", d, dim_arr[d]);
         #endif
      }

      n0 = 1;

      for (int n=0; n < nnthr; n++)
      {
         nthreads_arr[n] = n0;
         n0 *= 2;

         #ifdef DEBUG
         printf("nthreads_arr[%d] = %d\n", n, nthreads_arr[n]);
         #endif
      }
   }
   else if (argc == 3)
   { 
      dim_arr[0] = dim;
      nthreads_arr[0] = nthreads;
   }

   srand(time(NULL));

   for (int d=0; d < ndims; d++)
   {
      dim = dim_arr[d];

      mat1 = malloc(sizeof(int) * dim * dim);
      mat2 = malloc(sizeof(int) * dim * dim);

      if (!(mat1 && mat2))
      {
         puts("Falha na alocacao de memoria");
         return EXIT_FAILURE;
      }

      // Preenchendo matrizes de entrada com inteiros aleatórios:
      for (int i=0; i < dim; i++)
      {
         for (int j=0; j < dim; j++)
         {
            // #ifdef DEBUG
            // mat1[i * dim + j] = i * dim + j;
            // mat2[i * dim + j] = 1;
            // #else
            mat1[i * dim + j] = rand();
            mat2[i * dim + j] = rand();
            // #endif
         }
      }

      #ifdef DEBUG
      puts("Matrizes de entrada:\n");

      // display_matrix(dim, mat1);
      // display_matrix(dim, mat2);
      #endif
      

      for (int r=0; r < nruns; r++)
      {
         // Multiplicação sequencial:

         seq_out = malloc(sizeof(int) * dim * dim);

         if (!(seq_out))
         {
            puts("Falha na alocacao de memoria");
            return EXIT_FAILURE;
         }

         for (int i=0; i < dim; i++)
         {
            for (int j=0; j < dim; j++)
            {
               seq_out[i * dim + j] = 0;  // acumulador
            }
         }

         GET_TIME(t0);  // início da medição sequencial

         seq_mat_mul(dim, mat1, mat2, seq_out);

         GET_TIME(tf);  // fim da medição sequencial

         dt = tf - t0;

         #ifdef DEBUG
         printf("Duracao da etapa sequencial: %lf segundos\n", dt);
         #endif

         if (!r)
         // primeira iteração, t_seq não inicializado
         {
            t_seq[d] = dt;
         }
         else if (t_seq[d] > dt)
         // mínimo entre a medição atual e a menor anterior
         {
            t_seq[d] = dt;
         }
         
         // Início das iterações concorrentes:

         for (int n=0; n < nnthr; n++)
         {
            int idx = d * nnthr + n;
            nthreads = nthreads_arr[n];

            #ifdef DEBUG
            printf("dim_arr[%d] = %d\n", d, dim_arr[d]);
            printf("nthreads_arr[%d] = %d\n", n, nthreads_arr[n]);
            printf("dim = %d, nthreads = %d\n", dim, nthreads);
            #endif

            conc_out = malloc(sizeof(int) * dim * dim);

            nrange = malloc(sizeof(int) * nthreads);

            if (!(conc_out && nrange))
            {
               puts("Falha na alocacao de memoria");
               return EXIT_FAILURE;
            }

            // Preenchendo nrange com os identificadores "internos"
            // das threads:
            for (int t=0; t < nthreads; t++)
            {
               nrange[t] = t;
            }

            for (int i=0; i < dim; i++)
            {
               for (int j=0; j < dim; j++)
               { 
                  conc_out[i * dim + j] = 0;  // acumulador
               }
            }

            // Criação das threads:

            GET_TIME(t0);  // início da medição concorrente

            tid = (pthread_t*) malloc(sizeof(pthread_t*) * nthreads);
            
            if (!tid)
            {
               puts("Falha na alocacao de memoria");
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

            if (!r)
            // primeira iteração, t_conc não inicializado
            {
               t_conc[idx] = dt;
            }
            else if (t_conc[idx] > dt)
            // mínimo entre a medição atual e a menor anterior
            {
               t_conc[idx] = dt;
            }
            
            #ifndef DEBUG
            if (verify_conc_soln(dim, seq_out, conc_out))
            {
               return EXIT_FAILURE;
            }

            #else
            puts("Resultado sequencial:\n");
            // display_matrix(dim, seq_out);

            puts("\nResultado concorrente:\n");
            // display_matrix(dim, conc_out);
            #endif

            free(conc_out);
            free(nrange);
         }

         free(seq_out);
      }

      free(mat1);
      free(mat2);
   }
   
   // Salvando as medições em um arquivo csv:

   FILE* out_csv = fopen("perf_gains.csv", "w");

   // Cabeçalho do csv:

   fprintf(out_csv, "dim,");

   for (int n=0; n < nnthr; n++)
   {
      fprintf(out_csv, "%d thread", nthreads_arr[n]);

      if (nthreads_arr[n] > 1)
      {
         fprintf(out_csv, "s");
      }

      if (n < nnthr - 1)
      {
         fprintf(out_csv, ",");
      }
      else
      {
         fprintf(out_csv, "\n");
      }
   }

   double perf_gain;

   for (int d=0; d < ndims; d++)
   {
      fprintf(out_csv, "%d,", dim_arr[d]);

      for (int n=0; n < nnthr; n++)
      {
         perf_gain = t_seq[d] / t_conc[d * nnthr + n];

         #ifdef DEBUG
         printf("dim = %d, n = %d\n", dim_arr[d], nthreads_arr[n]);
         printf("perf_gain = %lf, t_seq[d] = %lf, t_conc[d * nnthr + n] = %lf\n", perf_gain, t_seq[d], t_conc[d * nnthr + n]);
         #endif

         fprintf(out_csv, "%lf", perf_gain);

         if (n < nnthr - 1)
         {
            fprintf(out_csv, ",");
         }
         else
         {
            fprintf(out_csv, "\n");
         }
      }
   }

   fclose(out_csv);

   free(dim_arr);
   free(nthreads_arr);

   free(t_seq);
   free(t_conc);
   
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

int verify_conc_soln(int dim, int* seq_out, int* conc_out)
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