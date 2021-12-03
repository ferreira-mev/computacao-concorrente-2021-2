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

void display_vec(float* vec, int nelem);

void init_vec(float* vec, int nelem);

// Fluxo da thread principal:

int main(int argc, char* argv[])
{
   srand(time(NULL));

   #ifdef DEBUG
   printf("Beginning execution\n");
   printf("sizeof(float) = %llu\n\n", sizeof(float));
   #endif

   if (argc == 5)
   { 
      nelem = atoll(argv[1]);
      lower_bound = atoll(argv[2]);
      upper_bound = atoll(argv[3]);
      nthreads = atoi(argv[4]);

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

      puts("Para usar um vetor de tamanho n, limiares inferior e");
      puts("superior, respectivamente, i e s, e t threads, execute\n");
      printf("%s n i s t\n\n", argv[0]);

      return EXIT_FAILURE;
   }

   vec = (float*) safe_malloc(sizeof(float) * nelem);

   init_vec(vec, nelem);

   display_vec(vec, nelem);

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
      puts("Falha na alocacao de memoria");
      exit(EXIT_FAILURE);
   }

   return ptr;
}

void display_vec(float* vec, int nelem)
/* Imprime um vetor vec de nelem elementos do tipo float. */
{
   puts("");

   for (int i=0; i < nelem; i++)
   {
      printf("%f ", *(vec + i));
   }

   puts("");
}

void init_vec(float* vec, int nelem)
/* Inicializa um vetor de nelem floats com entradas aleatórias. */
{
   for (int i=0; i < nelem; i++)
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
      printf("r = %d, *(vec + %d) = %.20f\n", r, i, *(vec + i));
      #endif
   }
}