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
long long int lower_bound;
long long int upper_bound;

// Estou usando limiares inteiros para evitar o problema de ler
// floats

int nthreads;



float* vec;

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

void display_fvec(float* vec, long long int nelem);
void display_dvec(int* vec, long long int nelem);

void init_vec(float* vec, long long int nelem);

// int seq_count_bounds(float* vec, long long int nelem);

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

   // Lendo e processando os parâmetros passados pela
   // linha de comando:

   if (argc == 3)
   {
      puts("Dois argumentos passados; entrando em modo de avaliacao");
      puts("de desempenho");

      n_nelem = 3;
      n_nthreads = 4;

      #ifdef DEBUG
      n_runs = 1;
      #else
      n_runs = 5;
      #endif

   }
   else if (argc == 5)
   { 
      nelem = atoll(argv[1]);
      lower_bound = atoll(argv[2]);
      upper_bound = atoll(argv[3]);
      nthreads = atoi(argv[4]);

      n_nelem = 1;
      n_nthreads = 1;
      n_runs = 1;

      #ifdef DEBUG
      printf("nelem = %lld\n", nelem);
      printf("lower_bound = %lld\n", lower_bound);
      printf("upper_bound = %lld\n", upper_bound);
      printf("nthreads = %d\n", nthreads);
      #endif

      if (nthreads > nelem)
      {
         printf("O numero de threads fornecido (%d) e maior que o\n", nthreads);
         printf("numero de elementos (%d) e sera reduzido para %d\n\n", nelem, nelem);

         nthreads = nelem;
         // (Eu podia só usar um min entre dim e nthreads, mas eu queria
         // o if para avisar ao usuário)
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

      display_dvec(nelem_arr, n_nelem);

      // TODO: tf does this print nelem_arr[0] 0 nelem_arr[1]?

      #else
      nelem_arr[0] = (int) pow(10, 5);
      nelem_arr[1] = (int) pow(10, 7);
      nelem_arr[2] = (int) pow(10, 8);
      #endif
   }

   // Inicializando o vetor de nelem floats aleatórios:

   vec = (float*) safe_malloc(sizeof(float) * nelem);

   init_vec(vec, nelem);

   #ifdef DEBUG
   display_fvec(vec, nelem);
   #endif

   free(vec);
   
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
   puts("");

   for (long long int i=0; i < nelem; i++)
   {
      printf("%f ", *(vec + i));
   }

   puts("");
}

void display_dvec(int* vec, long long int nelem)
/* Imprime um vetor vec de nelem elementos do tipo int. */
{
   puts("");

   for (long long int i=0; i < nelem; i++)
   {
      printf("%d ", *(vec + i));
   }

   puts("");
}

void init_vec(float* vec, long long int nelem)
/* Inicializa um vetor de nelem floats com entradas aleatórias. */
{
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