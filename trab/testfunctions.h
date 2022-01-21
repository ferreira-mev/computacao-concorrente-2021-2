/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções (matemáticas) para teste da integração numérica.
*/

#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

typedef double (*testfoo_ptr)(double);

double test_f1(double x);
double test_f2(double x);
double test_f3(double x);

double primitive_f1(double x);
double primitive_f2(double x);
double primitive_f3(double x);

double derivative_f1(double x);
double derivative_f2(double x);
double derivative_f3(double x);

#endif  // TESTFUNCTIONS_H