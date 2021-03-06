/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 3 -- Atividade 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "../lab02/timer.h"

// #define DEBUG

// Variáveis globais:

long long int nelem;
int lower_bound;
int upper_bound;

// Estou usando limiares inteiros para evitar o problema de ler
// floats (ponto, vírgula etc.)

int nthreads;
float* vec;

// Cabeçalhos de funções:

void* safe_malloc(size_t size);

void display_fvec(float* vec, long long int nelem);
void display_ffvec(double* vec, long long int nelem);
void display_dvec(int* vec, long long int nelem);
void display_llvec(long long int* vec, long long int nelem);

void init_vec(double* vec, long long int nelem, double value);
void init_rvec(float* vec, long long int nelem);

/* Mais ou menos em linha com um comentário que eu havia feito no
laboratório 2, nem todos esses parâmetros precisariam ser parâmetros,
já que estou usando variáveis globais, devido à concorrência. No 
início, os parâmetros acabaram entrando só pelo hábito de não usar esse
tipo de variável, mas depois pensei um pouco e concluí que achava mais
legível/organizado com os parâmetros ali, quando possível; fica mais
imediato quais variáveis estão sendo acessadas ou potencialmente
modificadas. 

No caso da contagem sequencial, no entanto, preferi acessar as
variáveis globais diretamente, para tornar a comparação com a contagem
concorrente mais fiel. */

long long int seq_bound_counter();
void* conc_bound_counter(void* arg);

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

   printf("\nsizeof(float) = %llu\n", sizeof(float));
   printf("sizeof(long long int) = %llu\n", sizeof(long long int));
   printf("sizeof(void*) = %llu\n", sizeof(void*));
   printf("RAND_MAX = %lld\n\n", RAND_MAX);
   #endif

   int* id_range;
   pthread_t* tid;
   int* thread_out;
   int conc_count;

   long long int* nelem_arr;
   int* nthreads_arr;

   int n_nelem;
   int n_nthreads;
   int n_runs;

   // P/ marcação de tempo:
   double t0, tf, dt;  // instantes inicial e final, e duração

   double* t_seq;
   double* t_conc;
   // Linhas: comprimento do vetor
   // Colunas: número de threads

   int idx;

   char csv_name[] = "perf_gains.csv";
   
   // Lendo e processando os parâmetros passados pela
   // linha de comando:

   if (argc == 3)
   {
      printf("Dois argumentos passados; entrando em modo de avaliacao");
      printf(" de desempenho");

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

   nelem_arr = (long long int*) safe_malloc(sizeof(long long int) * n_nelem);
   nthreads_arr = (int*) safe_malloc(sizeof(int) * n_nthreads);

   t_seq = (double*) safe_malloc(sizeof(double) * n_nelem);
   t_conc = (double*) safe_malloc(sizeof(double) * n_nelem * n_nthreads);

   init_vec(t_seq, n_nelem, -1);
   init_vec(t_conc, n_nelem * n_nthreads, -1);

   if (argc == 5)
   {
      nelem_arr[0] = nelem;
      nthreads_arr[0] = nthreads;
   }
   else if (argc == 3)
   { 
      #ifdef DEBUG
      nelem_arr[0] = 10;
      nelem_arr[1] = 11;
      nelem_arr[2] = 12;

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

      init_rvec(vec, nelem);

      #ifdef DEBUG
      printf("\nRandom vector of length %lld:\n", nelem);
      display_fvec(vec, nelem);
      #endif

      long long int seq_count = -1;

      for (int r=0; r < n_runs; r++)
      // p/ cada repetição da medição
      {
         #ifdef DEBUG
         printf("Beginning run %d\n", r + 1);
         #endif

         GET_TIME(t0);  // etapa sequencial

         seq_count = seq_bound_counter(vec, nelem, lower_bound, upper_bound);

         GET_TIME(tf);  // etapa sequencial
         dt = tf - t0;

         if (t_seq[n] < 0 || t_seq[n] > dt)
         // inicializado com -1; i.e., se a medição é a primeira ou a 
         // menor
         {
            t_seq[n] = dt;
         }

         #ifdef DEBUG
         printf("Sequential execution time: %lf s\n\n", dt);
         printf("Sequential result: %d\n", seq_count);
         #endif

         for (int t=0; t < n_nthreads; t++)
         // p/ cada valor do número de threads
         {
            GET_TIME(t0);  // etapa concorrente

            conc_count = 0;

            nthreads = nthreads_arr[t];

            #ifdef DEBUG
            printf("Beginning iteration: nthreads = %d\n", nthreads);
            #endif

            id_range = (int*) safe_malloc(sizeof(int) * nthreads);

            // Preenchendo id_range com os identificadores "internos"
            // das threads:
            for (int t=0; t < nthreads; t++)
            {
               id_range[t] = t;
            }

            #ifdef DEBUG
            printf("\nid_range: ");
            display_dvec(id_range, (long long int) nthreads);
            #endif

            // Criando as threads:
            tid = (pthread_t*) safe_malloc(sizeof(pthread_t*) * nthreads);

            for (int t=0; t < nthreads; t++)
            {
               if (pthread_create(tid + t, NULL, conc_bound_counter, (void*) (id_range + t)))
               {
                  fprintf(stderr, "Falha na criacao das threads");
                  return EXIT_FAILURE;
               }
            }

            for (int t=0; t < nthreads; t++)
            {
               if (pthread_join(*(tid + t), (void**) &thread_out))
               {
                  fprintf(stderr, "Falha na sincronizacao das threads");
                  return EXIT_FAILURE;
               }

               int thread_count = *thread_out;

               free(thread_out);

               #ifdef DEBUG
               printf("Thread %d returned %d\n", t, thread_count);
               #endif

               conc_count += thread_count;
            }
            
            #ifdef DEBUG
            printf("Sum over all threads: %d\n", conc_count);
            #endif

            GET_TIME(tf);  // etapa concorrente
            dt = tf - t0;

            idx = n * n_nthreads + t;

            if (t_conc[idx] < 0 || t_conc[idx] > dt)
            // inicializado com -1; i.e., se a medição é a primeira ou
            // a menor
            {
               t_conc[idx] = dt;
            }

            #ifdef DEBUG
            printf("Concurrent execution time (%d threads): %lf s\n\n", nthreads, dt);
            #endif

            long long int delta = seq_count - conc_count;

            #ifdef DEBUG
            printf("seq_out - conc_out = %lld\n", delta);
            #else
            if (delta)
            {
               fprintf(stderr, "\nFalha no programa concorrente: a");
               fprintf(stderr, " contagem de elementos\ndivergiu da ");
               fprintf(stderr, "do programa sequencial em %d\n", delta);

               return EXIT_FAILURE;
            }
            #endif

            free(id_range);
         } // p/ cada valor do número de threads

      } // p/ cada repetição da medição

      free(vec);
   } // p/ cada comprimento do vetor
   

   #ifndef DEBUG  // só para dar algum feedback ao usuário
   if (argc == 5)
   {
      printf("%lld elementos maiores que %d e menores que %d ", conc_count, lower_bound, upper_bound);
      printf("encontrados num vetor aleatório\nde tamanho %lld ", nelem_arr[0]);
      printf("numa contagem efetuada por %d thread(s)", nthreads_arr[0]);
      printf(" em %f s\n", dt);
   }
   else  // se chegou até aqui e não é 3, é 5
   {
      printf("\nResultados registrados em %s\n", csv_name);
   }
   #else
   puts("t_seq:");
   display_ffvec(t_seq, n_nelem);
   puts("t_conc:");
   display_ffvec(t_conc, n_nelem * n_nthreads);
   #endif

   // Salvando as medições em um arquivo csv:

   FILE* out_csv = fopen(csv_name, "w");

   // Cabeçalho do csv:

   fprintf(out_csv, "len,");

   for (int t=0; t < n_nthreads; t++)
   {
      fprintf(out_csv, "%d thread", nthreads_arr[t]);

      if (nthreads_arr[t] > 1)
      {
         fprintf(out_csv, "s");
      }

      if (t < n_nthreads - 1)
      {
         fprintf(out_csv, ",");
      }
      else
      {
         fprintf(out_csv, "\n");
      }
   }

   // Calculando e registrando os ganhos de perfomance:

   double perf_gain;

   for (int n=0; n < n_nelem; n++)
   {
      fprintf(out_csv, "%d,", nelem_arr[n]);

      for (int t=0; t < n_nthreads; t++)
      {
         idx = n * n_nthreads + t;

         perf_gain = t_seq[n] / t_conc[idx];

         #ifdef DEBUG
         printf("nelem = %d, n = %d\n", nelem_arr[n], nthreads_arr[t]);
         printf("perf_gain = %lf, t_seq[d] = %lf, t_conc[n * n_nthreads + t] = %lf\n", perf_gain, t_seq[n], t_conc[idx]);
         #endif

         fprintf(out_csv, "%lf", perf_gain);

         if (t < n_nthreads - 1)
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


   free(nelem_arr);
   free(nthreads_arr);

   free(t_seq);
   free(t_conc);
   
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

void display_ffvec(double* vec, long long int nelem)
/* Imprime um vetor vec de nelem elementos do tipo double. */
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

void init_vec(double* vec, long long int nelem, double value)
/* Inicializa um vetor de nelem floats double com entradas de valor 
value. */
{
   #ifdef DEBUG
   printf("\nInitializing vector of size %lld with all entries %f\n", nelem, value);
   #endif

   for (long long int i=0; i < nelem; i++)
   {
      *(vec + i) = value;

      #ifdef DEBUG
      printf("*(vec + %lld) = %.20f\n", i, *(vec + i));
      #endif
   }
}

void init_rvec(float* vec, long long int nelem)
/* Inicializa um vetor de nelem floats com entradas aleatórias. */
{
   #ifdef DEBUG
   printf("\nInitializing random vector of size %lld\n", nelem);
   #endif

   float val;

   for (long long int i=0; i < nelem; i++)
   {
      int r = rand();
      if (!r) { val = (float) 0; }
      else { val = 1000000000.0 / r; }

      // Não foi especificado como deveríamos gerar os floats; por
      // experimentação, observei que, dessa forma, tinha partes
      // inteiras não todas zero, mas com casas decimais até o limite
      // da precisão (diferentemente do que acontece, por exemplo, ao se
      // dividir o inteiro retornado por rand() por uma constante).

      // Randomizando o sinal, para ficar mais interessante (rand() 
      // retorna apenas inteiros positivos):

      int s = rand();
      
      if (s >= RAND_MAX / 2) { val *= -1.0; }

      *(vec + i) = val;

      #ifdef DEBUG
      printf("r = %d, *(vec + %lld) = %.20f\n", r, i, *(vec + i));
      #endif
   }
}

long long int seq_bound_counter()
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

void* conc_bound_counter(void* arg)
/* Função concorrente para contar o número de entradas do vetor de 
floats vec, de comprimento nelem, que estão entre lower_bound e 
upper_bound, exclusive; i.e., vec[i] é contado se
lower_bound < vec[i] < upper_bound. */
{
   int id = *((int*) arg);

   #ifdef DEBUG
   printf("Function call for thread %d\n", id);
   #endif

   long long int* thread_count;

   thread_count = (long long int*) safe_malloc(sizeof(long long int));
   *thread_count = 0;

   long long int chunk_size = nelem / (long long int) nthreads;
   long long int init_pos = id * chunk_size;
   long long int end_pos;
   
   if (id == nthreads - 1) { end_pos = nelem; }
   else { end_pos = (id + 1) * chunk_size; }

   #ifdef DEBUG
   if (!id)
   {
      printf("Computed chunk size: %lld\n", chunk_size);
   }
   printf("Looping over positions %lld to %lld\n", init_pos, end_pos);
   #endif

   for (int pos = init_pos; pos < end_pos; pos++)
   {
      #ifdef DEBUG
      printf("Thread %d found element %lld: %f\n", id, pos, *(vec + pos));
      #endif

      if
      (
         (*(vec + pos) > (float) lower_bound) &&
         (*(vec + pos) < (float) upper_bound)
      )
      {
         (*thread_count)++;

         #ifdef DEBUG
         printf("Thread %d match! Count updated to %lld\n", id, *thread_count);
         #endif
      }
      
      #ifdef DEBUG
      puts("");
      #endif
   }

   #ifdef DEBUG
   printf("Final position reached; thread %d total: %lld\n\n", id, *thread_count);
   #endif

   pthread_exit((void*) thread_count);
}
