/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções (matemáticas) para teste da integração numérica.
*/

#include <math.h>
#include "testfunctions.h"

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