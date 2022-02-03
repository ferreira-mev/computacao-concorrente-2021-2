/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 10 -- Atividade 1

Minibiblioteca para geração de logs.
*/

#ifndef LOGGING_H
#define LOGGING_H

char* get_timestamp();
char* logname(int n_prod, int n_cons);
void printlog(FILE* logfile, int thread_id, int should_print, char* message);

#endif // LOGGING_H