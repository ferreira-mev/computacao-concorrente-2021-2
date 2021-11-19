/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 1 -- Atividade 5

Programa que eleva ao quadrado os elementos de um vetor, alterando o vetor inicial, com o uso de duas threads paralelas além do programa principal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2  // número de threads (além do main)