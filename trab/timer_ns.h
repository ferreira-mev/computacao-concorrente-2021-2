/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Este é o arquivo timer.h que havia sido fornecido pela professora para 
uso no laboratório 2. Como eu o editei para fazer medidas em 
nanossegundos em vez de segundos, achei melhor incluir uma versão 
separada e manter a anterior como estava na pasta do lab 2, para evitar 
inconsistências caso a correção dos labs 2 e 3 seja feita após um pull
da versão mais recente do repositório. (Pode não ser óbvio verificar
se esse arquivo teve alguma alteração posterior para revertê-la.) 
*/

/* Example:  
 *    #include "timer.h"
 *    . . .
 *    double start, finish, elapsed;
 *    . . .
 *    GET_TIME(start);
 *    . . .
 *    Code to be timed
 *    . . .
 *    GET_TIME(finish);
 *    elapsed = finish - start;
 *    printf("The code to be timed took %e seconds\n", elapsed);
*/

#ifndef _CLOCK_TIMER_H
#define _CLOCK_TIMER_H

#include <sys/time.h>
#define BILLION 1000000000L

/* The argument now should be a double (not a pointer to a double) */
#define GET_TIME(now) { \
   struct timespec time; \
   clock_gettime(CLOCK_MONOTONIC, &time); \
   now = (BILLION * time.tv_sec) + time.tv_nsec; \
}
#endif
//now = time.tv_sec + time.tv_nsec/1000000000.0; //seconds
// now = (BILLION * time.tv_sec) + time.tv_nsec; //nanoseconds
