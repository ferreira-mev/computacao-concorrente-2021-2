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

#include "../lab02/timer.h"
#include "testfunctions.h"
#include "helperfunctions.h"

#define N_THREADS_LEN 6
#define N_SUBS_LEN 4
#define N_TEST_FOOS 3
#define N_RUNS 5

int n_threads[N_THREADS_LEN] = { 1, 2, 4, 6, 8, 10 };
int n_subintervals[N_SUBS_LEN] = { 1000, 10000, 100000, 1000000 };

testfoo_ptr test_functions[N_TEST_FOOS] =
{
    test_f1, test_f2, test_f3
};

testfoo_ptr test_primitives[N_TEST_FOOS] =
{
    primitive_f1, primitive_f2, primitive_f3
};

testfoo_ptr test_derivatives[N_TEST_FOOS] =
{
    derivative_f1, derivative_f2, derivative_f3
};

typedef struct
{
    int thread_id;
    int n_threads;
    int function_id;
    int n_subintervals;
} thread_args;

double min_x = 1.0;
double max_x = 11.0;  // integração em [1, 11]

void* integrate_subinterval(void* arg);

double integrate_seq(int f_id, int curr_n_subs);

/* Desta vez, diferentemente do que havia comentado em um laboratório 
anterior, eu optei por NÃO tentar deixar as funções sequencial e
concorrente o mais semelhantes o possível, no tocante a argumentos etc.
Tomei essa decisão porque considerei que, no caso de uma implementação
apenas sequencial, não haveria nenhum motivo para, por exemplo, usar 
structs ou variáveis globais em lugar de múltiplos argumentos. Como 
essas escolhas de implementação são forçadas pela concorrência, achei 
que faria sentido medir qualquer (des)vantagem introduzida por elas 
em termos de tempo de execução, para me aproximar melhor de uma 
observação real das vantagens "líquidas" oferecidas pela concorrência,
descontados seus "custos". */

// Fluxo da thread principal:

int main(int argc, char *argv[])
{
    puts("[main] Iniciando execucao");

    pthread_t* tid;  // identificadores internos das threads
    double seq_out, conc_out;  // resultados sequencial e concorrente
    double* thread_out;  // recebe a saída de cada thread
    double analytical, h;  // p/ comparar conc e "analítico"

    double delta = pow(10, -9);  // p/ comparar valores double

    // P/ marcação de tempo:
    double t0, tf, dt;  // instantes inicial e final, e duração

    char csv_dir[] = "csvs";  // diretório p/ salvar as medidas

    double t_seq[N_TEST_FOOS][N_SUBS_LEN] = {0};
    double t_conc[N_TEST_FOOS][N_SUBS_LEN][N_THREADS_LEN] = {0};
    // arrays multid preenchidos com acumuladores, para somar
    // N_RUNS medidas e extrair a média

    for (int f_idx = 0; f_idx < N_TEST_FOOS; f_idx++)  // p/ cada função de teste
    {
        FILE* time_file = fopen(csv_filename(csv_dir, "time", f_idx), "w");
        // csv p/ armazenar medidas de tempo referentes a essa f
        FILE* acc_file = fopen(csv_filename(csv_dir, "acc", f_idx), "w");
        // csv p/ armazenar medidas de aceleração referentes a essa f

        if (!time_file || !acc_file)
        {
            fprintf(stderr, "Falha na geracao dos arquivos de saida\n");
            exit(EXIT_FAILURE);   
        }  // else implícito

        // Cabeçalho dos csvs:

        FILE* file_ptrs[] = {time_file, acc_file};

        for (int i = 0; i < 2; i++)  // p/ cada arquivo csv de saída
        {
            FILE* curr_file = file_ptrs[i];

            fprintf(curr_file, "n");

            if (!i)  // time_file (pouco abstrato, mas vá; é C anyway :P)
            {
                fprintf(curr_file, ",*");  // p/ o tempo sequencial
            }

            for (int t_idx = 0; t_idx < N_THREADS_LEN; t_idx++)  // p/ cada qtd de threads
            // (não dá pra aproveitar o outro loop porque preciso do 
            // cabeçalho primeiro)
            {
                fprintf(curr_file, ",%d", n_threads[t_idx]);
            }

            fprintf(curr_file, "\n");
        }  // p/ cada arquivo csv de saída

        for (int n_idx = 0; n_idx < N_SUBS_LEN; n_idx++)  // p/ cada qtd de subintervalos
        {
            for (int t_idx = 0; t_idx < N_THREADS_LEN; t_idx++)  // p/ cada qtd de threads
            {
                for (int r = 0; r < N_RUNS; r++)  // p/ cada repetição da medição
                {
                    if (!t_idx)
                    {
                        // não preciso repetir a execução sequencial
                        // p/ cada qtd de threads
                        
                        GET_TIME(t0);  // início da medição sequencial
                        seq_out = integrate_seq(f_idx, n_subintervals[n_idx]);
                        GET_TIME(tf);  // fim da medição sequencial

                        dt = tf - t0;

                        t_seq[f_idx][n_idx] += dt;
                    }
                    GET_TIME(t0);  // início da medição concorrente

                    int max_t = n_threads[t_idx];
                    conc_out = 0;  // acumulador

                    // Criando as threads:

                    tid = (pthread_t*) safe_malloc(sizeof(pthread_t*) * max_t);

                    for (int t = 0; t < max_t; t++)
                    {
                        thread_args* curr_args;

                        curr_args = (thread_args*) safe_malloc(sizeof(thread_args));

                        curr_args->thread_id = t;
                        curr_args->n_threads = n_threads[t_idx];
                        curr_args->function_id = f_idx;
                        curr_args->n_subintervals = n_subintervals[n_idx];

                        if (pthread_create(tid + t, NULL, integrate_subinterval, (void*) curr_args))
                        {
                            fprintf(stderr, "Falha na criacao das threads");
                            return EXIT_FAILURE;
                        }

                    }

                    // Sincronizando as threads:

                    for (int t = 0; t < max_t; t++)
                    {
                        if (pthread_join(*(tid + t), (void**) &thread_out))
                        {
                            fprintf(stderr, "Falha na sincronizacao das threads");
                            return EXIT_FAILURE;
                        }

                        conc_out += *(thread_out);

                        free(thread_out);  // precisa? alocada na tarefa da thread
                    }
                    
                    free(tid);

                    GET_TIME(tf);  // fim da medição concorrente

                    dt = tf - t0;

                    t_conc[f_idx][n_idx][t_idx] += dt;

                    // Verificação da corretude:

                    char case_string[100];  // overkill de tamanho

                    sprintf
                    (
                        case_string,
                        "no caso da funcao %d com %d subintervalos e %d threads",
                        f_idx + 1,
                        n_subintervals[n_idx],
                        n_threads[t_idx]
                    );

                    // Comparando resultados sequencial e concorrente:

                    if (!compare_doubles(seq_out, conc_out, delta))
                    {
                        printf("[main] Erro %s,\n", case_string);
                        puts("[main] na comparacao com o resultado sequencial:");
                        printf("[main] Saida concorrente: %f\n", conc_out);
                        printf("[main] Saida sequencial: %f\n", seq_out);
                        printf("[main] (Diferença absoluta: %f)\n", fabs(seq_out - conc_out));

                        puts("");

                    }

                    /* Comparando resultado concorrente com a integral
                    calculada a partir da primitiva analítica, somada ao
                    maior termo do erro de aproximação esperado: */

                    analytical = test_primitives[f_idx](max_x) - test_primitives[f_idx](min_x);

                    h = (max_x - min_x) / n_subintervals[n_idx];
                    // comprimento do subintervalo

                    // Uma observação: estou calculando esse h em três 
                    // lugares: aqui, na implementação sequencial e
                    // na concorrente. No entanto, como ele muda com
                    // as iterações, achei melhor fazer isso do que usar
                    // uma variável global e precisar me preocupar com
                    // o controle do acesso a ela; por exemplo, uma 
                    // thread criada com um determinado valor de n
                    // poderia ler h quando a thread principal já
                    // o tivesse recalculado com outro n.

                    analytical += pow(h, 2) * (test_derivatives[f_idx](max_x) - test_derivatives[f_idx](min_x)) / 12;
                    // cálculo direto a partir do resultado analítico,
                    // acrescido do maior termo do erro de aproximação 
                    // (também analítico, distinto do erro numérico
                    // introduzido pelo limite da precisão da máquina;
                    // vide relatório)

                    if (!compare_doubles(analytical, conc_out, delta))
                    {
                        printf("[main] Erro %s,\n", case_string);
                        puts("[main] na comparacao com o resultado \"analitico\":");
                        printf("[main] Saida concorrente: %f\n", conc_out);
                        printf("[main] Resultado \"analitico\" + maior termo do erro: %f\n", analytical);
                        printf("[main] (Diferença absoluta: %f)\n", fabs(analytical - conc_out));

                        puts("");
                    }
                    
                }  // p/ cada repetição da medição

                if (!t_idx)
                {
                    t_seq[f_idx][n_idx] /= N_RUNS;
                }

                t_conc[f_idx][n_idx][t_idx] /= N_RUNS;

            }  // p/ cada qtd de threads

            // Registrando o valor de n:

            for (int i = 0; i < 2; i++)
            {
                FILE* curr_file = file_ptrs[i];

                fprintf(curr_file, "%d", n_subintervals[n_idx]);
            }
    
            // Registrando o tempo medido:

            for (int t_idx = -1; t_idx < N_THREADS_LEN; t_idx++)  // p/ cada qtd de threads + 1
            {
                if (t_idx < 0)
                // não há aceleração referente ao caso sequencial
                // (preferi não incluir uma coluna só de 1s em toda
                // tabela)
                {
                    fprintf(time_file, ",%f", t_seq[f_idx][n_idx]);
                }
                else
                {
                    fprintf(time_file, ",%f", t_conc[f_idx][n_idx][t_idx]);

                    // Calculando e registrando a aceleração:

                    double acc = t_seq[f_idx][n_idx] / t_conc[f_idx][n_idx][t_idx];
                    fprintf(acc_file, ",%f", acc);
                }
            }

            for (int i = 0; i < 2; i++)
            {
                FILE* curr_file = file_ptrs[i];

                fprintf(curr_file, "\n");
            }

        }  // p/ cada qtd de subintervalos

        for (int i = 0; i < 2; i++)
        {
            FILE* curr_file = file_ptrs[i];
            fclose(curr_file);
        }

    }  // p/ cada função de teste

    puts("[main] Encerrando execucao com sucesso");

    return EXIT_SUCCESS;
}


double integrate_seq(int f_id, int curr_n_subs)
/* Implementação sequencial do método do trapézio. */
{
    double h = (max_x - min_x) / curr_n_subs;
    double output = 0.0;

    double min_xk, max_xk;
    // extremos do intervalo em cada iteração
    min_xk = min_x;
    double min_f, max_f;
    // valores da função nos extremos
    min_f = test_functions[f_id](min_xk);

    // Aplicando a fórmula do método do trapézio a
    // cada subintervalo que compete à thread:

    for (int k = 0; k < curr_n_subs; k++)
    {
        max_xk = min_xk + h;
        max_f = test_functions[f_id](max_xk);

        output += min_f + max_f;

        min_xk = max_xk;
        min_f = max_f;
    }

    output *= h / 2;

    return output;
}


void* integrate_subinterval(void* arg)
/* Tarefa executada por cada thread na implementação
concorrente do método do trapézio. */
{
    thread_args args = *((thread_args*) arg);

    // "Unpacking" os argumentos:

    int id = args.thread_id;
    int curr_n_thr = args.n_threads;
    int f_id = args.function_id;
    int curr_n_subs = args.n_subintervals;
    
    double h = (max_x - min_x) / curr_n_subs;
    // tamanho do subintervalo

    int chunk_size = curr_n_subs / curr_n_thr;
    // subintervalos por thread

    int max_iter = chunk_size;  // número de iterações

    if (id == curr_n_thr - 1)
    {
        max_iter += curr_n_subs % curr_n_thr;
        // se for a última thread, seguir até o fim
    }

    double* output;  // para retornar
    output = (double*) safe_malloc(sizeof(double));
    *output = 0.0;  // acumulador

    double min_xk, max_xk;
    // extremos do intervalo em cada iteração
    min_xk = min_x + id * h * chunk_size;
    double min_f, max_f;
    // valores da função nos extremos
    min_f = test_functions[f_id](min_xk);

    // Aplicando a fórmula do método do trapézio a
    // cada subintervalo que compete à thread:

    for (int k = 0; k < max_iter; k++)
    {
        max_xk = min_xk + h;
        max_f = test_functions[f_id](max_xk);

        *output += min_f + max_f;

        min_xk = max_xk;
        min_f = max_f;
    }

    *output *= h / 2;
    // fazendo isso por último para diminuir a quantidade de
    // operações, e, com isso, o erro numérico (coloquei o
    // fator comum em evidência)

    free(arg);
    // (percebi que tinha que liberar aqui dentro, ou corria
    // o risco de a main liberar antes de a thread terminar de
    // usar hihi)

    pthread_exit((void*) output);
}