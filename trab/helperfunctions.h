/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções auxiliares, diferentes das funções matemáticas de teste e dos
fluxos de execução das threads principal e paralelas.
*/

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>

void* safe_malloc(size_t size);

#endif // HELPERFUNCTIONS_H