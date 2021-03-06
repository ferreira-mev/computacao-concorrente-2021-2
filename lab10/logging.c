/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 10 -- Atividade 1

Minibiblioteca para geração de logs.

Formato do nome do arquivo:
Sem <n>cons <n>prod <timestamp>.log

Formato de impressão:
[<timestamp>] [<número da thread>] <mensagem>
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "logging.h"
#include "helperfunctions.h"

char* get_timestamp()
/* Retorna uma timestamp referente ao momento atual idêntica à
retornada por ctime(), porém sem a quebra de linha final. */
{
    char* timestamp = (char*) safe_malloc(sizeof(char) * 100);

    time_t now;
    time(&now);
    sprintf(timestamp, "%s.log", strtok(ctime(&now), "\n"));
    // https://stackoverflow.com/a/6509569

    return timestamp;
}

char* logname(int n_cons, int n_prod)
/* Retorna o nome do arquivo de log no formato
Sem <n>cons <n>prod <timestamp>.log
*/
{
    char* name = (char*) safe_malloc(sizeof(char) * 100);
    char* timestamp = get_timestamp();

    sprintf(name, "Sem %dcons %dprod ", n_cons, n_prod);

    strcat(name, timestamp);

    free(timestamp);

    return name;
}

void printlog(FILE* logfile, int thread_id, int should_print, char* message)
/* Imprime message no arquivo logfile, identificada por thread_id.
Convenciona-se que a thread main tem ID -1.

Imprime identicamente em stdin caso should_print não seja 0.*/
{
    char* timestamp = get_timestamp();

    if (thread_id >= 0)
    {
        fprintf(logfile, "[%s] [%d] %s\n", timestamp, thread_id, message);
        if (should_print)
        {
            printf("[%s] [%d] %s\n", timestamp, thread_id, message);
        }
    }
    else
    {
        fprintf(logfile, "[%s] [main] %s\n", timestamp, message);

        if (should_print)
        {
            printf("[%s] [main] %s\n", timestamp, message);
        }
    }

    free(timestamp);
}