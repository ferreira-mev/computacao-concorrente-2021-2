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

int* vec;

// Cabeçalhos de funções:

void display_vec(int* vec, int nelem);

void init_vec(int* vec);

// Fluxo da thread principal:

int main(int argc, char* argv[])
{  
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
   
   return EXIT_SUCCESS;
}

// Definições de funções:

void display_vec(int* vec, int nelem)
/* Imprime um vetor vec de nelem elementos. */
{

}