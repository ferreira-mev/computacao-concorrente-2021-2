/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Paralelização da integração numérica pelo método do trapézio.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// enum FUNCTIONS
// {
//     poly,
//     exp,
//     trig
// };  // actually less abstract?

typedef double (*testfoo_ptr)(double);

// Cabeçalhos de funções (matemáticas) de teste:

double test_f1(double x);
double test_f2(double x);
double test_f3(double x);

double primitive_f1(double x);
double primitive_f2(double x);
double primitive_f3(double x);

testfoo_ptr test_functions[3] =
{
    test_f1, test_f2, test_f3
};

testfoo_ptr test_primitives[3] =
{
    primitive_f1, primitive_f2, primitive_f3
};

// Cabeçalhos de outras funções:

void* safe_malloc(size_t size);

// Fluxo da thread principal:

int main(int argc, char *argv[])
{
    puts("[main] Iniciando execucao");

    printf("[main] Testando: f1(0) = %f\n", test_functions[2](0));

    printf("[main] Testando: F2(1) = %f\n", test_primitives[1](1));

    puts("[main] Encerrando execucao com sucesso");

    return EXIT_SUCCESS;
}

// Definições de funções (matemáticas) de teste:

double test_f1(double x)  // 4x^5 + x^2 - 3x - 5
{
    return 4*pow(x, 5) + pow(x, 2) - 3*x + 5;
}

double test_f2(double x)
{
    return 1 / x;
}

double test_f3(double x)
{
    return cos(2*x);
}

double primitive_f1(double x)  // 2/3 x^6 + x^3 / 3 - 3/2 x^2 - 5x
{
    return 2/3 * pow(x, 6) + pow(x, 3) / 3 - 3/2 * pow(x, 2) - 5*x;
}

double primitive_f2(double x)
{
    return log(x);
}

double primitive_f3(double x)
{
    return sin(x) * cos(x);
}

// Definições de outras funções:

void* safe_malloc(size_t size)
/* "Wrap" para a função malloc com verificação de falha na alocação. */
{
   void* ptr = malloc(size);

   if (!ptr)
   {
      fprintf(stderr, "Falha na alocacao de memoria\n");
      exit(EXIT_FAILURE);
   }

   return ptr;
}