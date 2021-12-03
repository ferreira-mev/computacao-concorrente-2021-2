/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 3 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "../lab02/timer.h"

#define DEBUG

// Variáveis globais:

long long int nelem;
int lower_bound;
int upper_bound;

// Estou usando limiares inteiros para evitar o problema de ler
// floats

int nthreads;

float* vec;

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

void display_fvec(float* vec, long long int nelem);
void display_dvec(int* vec, long long int nelem);
void display_llvec(long long int* vec, long long int nelem);

void init_vec(float* vec, long long int nelem);

long long int seq_bound_counter
(
   float* vec,
   long long int nelem,
   int lower_bound,
   int upper_bound
);

/* Mais ou menos em linha com um comentário que eu havia feito no
laboratório 2, nem todos esses parâmetros precisariam ser parâmetros,
já que estou usando variáveis globais, devido à concorrência. No 
início, os parâmetros acabaram entrando só pelo hábito de não usar esse
tipo de variável, mas depois pensei um pouco e concluí que achava mais
legível/organizado com os parâmetros ali, quando possível; fica mais
imediato quais variáveis estão sendo acessadas ou potencialmente
modificadas. */

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   srand(time(NULL));

   #ifdef DEBUG
   printf("Beginning execution of ");
   for (int i=0; i < argc; i++)
   {
      printf("%s ", argv[i]);
   }

   printf("\nsizeof(float) = %llu\n\n", sizeof(float));
   #endif

   long long int* nelem_arr;
   int* nthreads_arr;

   int n_nelem;
   int n_nthreads;
   int n_runs;

   double t0, tf, dt;  // instantes inicial e final, e duração

   // Lendo e processando os parâmetros passados pela
   // linha de comando:

   if (argc == 3)
   {
      puts("Dois argumentos passados; entrando em modo de avaliacao");
      puts("de desempenho");

      n_nelem = 3;
      n_nthreads = 4;

      #ifdef DEBUG
      n_runs = 2;
      #else
      n_runs = 5;
      #endif

      lower_bound = atoi(argv[1]);
      upper_bound = atoi(argv[2]);

      #ifdef DEBUG
      printf("lower_bound = %d\n", lower_bound);
      printf("upper_bound = %d\n", upper_bound);
      #endif

   }
   else if (argc == 5)
   { 
      nelem = atoll(argv[1]);
      lower_bound = atoi(argv[2]);
      upper_bound = atoi(argv[3]);
      nthreads = atoi(argv[4]);

      n_nelem = 1;
      n_nthreads = 1;
      n_runs = 1;

      #ifdef DEBUG
      printf("nelem = %lld\n", nelem);
      printf("lower_bound = %d\n", lower_bound);
      printf("upper_bound = %d\n", upper_bound);
      printf("nthreads = %d\n", nthreads);
      #endif

      if (nthreads > nelem)
      {
         printf("O numero de threads fornecido (%d) e maior que o\n", nthreads);
         printf("numero de elementos (%d) e sera reduzido para %d\n\n", nelem, nelem);

         nthreads = nelem;
      }
   }
   else
   {
      puts("Modo de uso:\n");

      puts("Para usar um vetor de tamanho n; limiares inferior e");
      puts("superior, respectivamente, i e s; e t threads, execute\n");
      printf("%s n i s t\n\n", argv[0]);

      puts("Para medir o desempenho com tamanhos de vetores e");
      puts("números de threads diversos, ainda usando limiares");
      puts("inferior e superior dados por i e s, execute \n");
      printf("%s i s \n\n", argv[0]);

      return EXIT_FAILURE;
   }

   #ifdef DEBUG
   printf("\nn_nelem = %d, n_nthreads = %d, n_runs = %d\n", n_nelem, n_nthreads, n_runs);
   #endif

   // Preenchendo os arrays com os números de elementos e threads a
   // serem usados:

   nelem_arr = safe_malloc(sizeof(long long int) * n_nelem);
   nthreads_arr = safe_malloc(sizeof(int) * n_nthreads);

   if (argc == 5)
   {
      nelem_arr[0] = nelem;
      nthreads_arr[0] = nthreads;
   }
   else if (argc == 3)
   { 
      #ifdef DEBUG
      nelem_arr[0] = 3;
      nelem_arr[1] = 5;
      nelem_arr[2] = 10;

      puts("Elementos de nelem_arr:");
      display_llvec(nelem_arr, n_nelem);

      #else
      nelem_arr[0] = (int) pow(10, 5);
      nelem_arr[1] = (int) pow(10, 7);
      nelem_arr[2] = (int) pow(10, 8);
      #endif

      int n0 = 1;

      for (int n=0; n < n_nthreads; n++)
      {
         nthreads_arr[n] = n0;
         n0 *= 2;
      }

      #ifdef DEBUG
      puts("Elementos de nthreads_arr:");
      display_dvec(nthreads_arr, n_nthreads);
      #endif
   }


   for (int n=0; n < n_nelem; n++)
   // p/ cada comprimento do vetor
   {
      nelem = nelem_arr[n];

      #ifdef DEBUG
      printf("\nBeginning iteration: nelem = %lld\n", nelem);
      #endif

      // Inicializando o vetor de nelem floats aleatórios:

      vec = (float*) safe_malloc(sizeof(float) * nelem);

      init_vec(vec, nelem);

      #ifdef DEBUG
      printf("\nRandom vector of length %lld:", nelem);
      display_fvec(vec, nelem);
      #endif

      long long int seq_count = -1;

      for (int r=0; r < n_runs; r++)
      // p/ cada repetição da medição
      {
         #ifdef DEBUG
         printf("Beginning run %d\n", r + 1);
         #endif

         long long int seq_count_r;

         GET_TIME(t0);

         seq_count_r = seq_bound_counter(vec, nelem, lower_bound, upper_bound);

         GET_TIME(tf);
         dt = tf - t0;

         #ifdef DEBUG
         printf("Sequential execution time: %lf s\n\n", dt);
         #endif

         if (seq_count == -1)
         { 
            seq_count = seq_count_r;

            // Não sei se faz diferença, mas pensei que poderia ser
            // melhor comparar todas as saídas concorrentes com a
            // mesma saída sequencial. P/ cada tamanho de vetor,
            // a contagem sequencial é armazenada apenas na primeira
            // execução; as seguintes são apenas para repetir a
            // medição de tempo, cf. solicitado nas instruções.
         }

         for (int t=0; t < n_nthreads; t++)
         // p/ cada valor do número de threads
         {
            nthreads = nthreads_arr[t];

            #ifdef DEBUG
            printf("Beginning iteration: nthreads = %d\n", nthreads);
            #endif
         } // p/ cada valor do número de threads

      } // p/ cada repetição da medição

      free(vec);
   } // p/ cada comprimento do vetor
   

   
   
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

void display_fvec(float* vec, long long int nelem)
/* Imprime um vetor vec de nelem elementos do tipo float. */
{
   // puts("");

   for (long long int i=0; i < nelem; i++)
   {
      printf("%f ", *(vec + i));
   }

   puts("");
}

void display_dvec(int* vec, long long int nelem)
/* Imprime um vetor vec de nelem elementos do tipo int. */
{
   // puts("");

   for (long long int i=0; i < nelem; i++)
   {
      printf("%d ", *(vec + i));
   }

   puts("");
}

void display_llvec(long long int* vec, long long int nelem)
/* Imprime um vetor vec de nelem elementos do tipo long long int. */
{
   // puts("");

   for (long long int i=0; i < nelem; i++)
   {
      printf("%lld ", *(vec + i));
   }

   puts("");
}

// (e aqui eu queria que o C tivesse o overloading do C++ :P)

void init_vec(float* vec, long long int nelem)
/* Inicializa um vetor de nelem floats com entradas aleatórias. */
{
   #ifdef DEBUG
   printf("\nInitializing random vector of size %lld\n", nelem);
   #endif

   for (long long int i=0; i < nelem; i++)
   {
      int r = rand();
      if (!r) { *(vec + i) = (float) 0; }
      else { *(vec + i) = 1000000000.0 / r; }

      // Não foi especificado como deveríamos gerar os floats; por
      // experimentação, observei que, dessa forma, tinha partes
      // inteiras não todas zero, mas com casas decimais o suficiente
      // para observar a diferença no erro numérico entre os casos
      // sequencial e concorrente (diferentemente do que acontece, por
      // exemplo, ao se dividir por uma constante).

      #ifdef DEBUG
      printf("r = %d, *(vec + %lld) = %.20f\n", r, i, *(vec + i));
      #endif
   }
}

long long int seq_bound_counter
(
   float* vec,
   long long int nelem,
   int lower_bound,
   int upper_bound
)
/* Função sequencial para contar o número de entradas do vetor de floats
vec, de comprimento nelem, que estão entre lower_bound e upper_bound, 
exclusive; i.e., vec[i] é contado se
lower_bound < vec[i] < upper_bound. */
{
   long long int count = 0;

   #ifdef DEBUG
   printf("\nRunning sequential count\n");
   #endif

   for (long long int i = 0; i < nelem; i++)
   {
      #ifdef DEBUG
      printf("Found element %lld: %f", i, *(vec + i));
      #endif

      if
      (
         (*(vec + i) > (float) lower_bound) &&
         (*(vec + i) < (float) upper_bound)
      )
      {
         count++;

         #ifdef DEBUG
         printf("; match! Count updated to %lld", count);
         #endif
      }
      
      #ifdef DEBUG
      puts("");
      #endif
   }

   #ifdef DEBUG
   puts("End of vector reached\n");
   #endif

   return count;
}