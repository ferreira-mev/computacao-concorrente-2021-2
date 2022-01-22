/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira

Trabalho de Implementação

Funções auxiliares, diferentes das funções matemáticas de teste e dos
fluxos de execução das threads principal e paralelas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "helperfunctions.h"

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

short int compare_doubles(double x, double y, double delta)
/* Retorna 1 se os números de ponto flutuante x e y podem ser
considerados iguais por uma margem delta, e 0 caso contrário.

Adaptada de https://floating-point-gui.de/errors/comparison/ */
{
    double abs_x = fabs(x);
    double abs_y = fabs(y);
    double abs_diff = fabs(x - y);
    
    if (x == y)
    {
        return 1;
    }
    else if (!x || !y || (abs_x + abs_y < DBL_MIN))
    {
        return abs_diff < (delta * DBL_MIN);
    }
    else
    {
        return abs_diff / (abs_x + abs_y) < delta;
    }
}

char* csv_filename(char* csv_dir, char* suffix, int f_id)
/* Retorna o nome do arquivo csv para registro dos dados medidos,
dados o diretório csv_dir (que deve ser uma string vazia para salvar
no diretório atual) e o identificador f_id do caso de teste. */
{
    char* filename;
    filename = (char*) safe_malloc(sizeof(char) * 100);
    // overkill de tamanho

    sprintf(filename, "f%d_%s.csv", f_id + 1, suffix);

    if (csv_dir[0] == '\0')  // string vazia
    {
        return filename;
    }
    else
    {
        char* filename_with_path;
        filename_with_path = (char*) safe_malloc(sizeof(char) * 256);
        // overkill de tamanho

        sprintf(filename_with_path, "%s/", csv_dir);
        strcat(filename_with_path, filename);

        return filename_with_path;
    }
}