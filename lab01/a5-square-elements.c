/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 1 -- Atividade 5

Programa que eleva ao quadrado os elementos de um vetor de inteiros, 
alterando o vetor inicial, com o uso de duas threads paralelas além do
programa principal.

O vetor de entrada deve ser fornecido num arquivo de texto contendo uma 
entrada por linha, com a última linha em branco. Esse arquivo deve ser
passado ao programa principal pela linha de comando, por meio do
redirecionamento de stdin; por exemplo:

./a5-square-elements.exe < test_vector.txt

A makefile incluída permite a execução de um teste com o vetor contendo
os inteiros de -3 a 9996 executando make test.

Obs: Executei alguns testes manuais variando NTHREADS, porém com NELEMS
reduzido, para facilitar a entrada manual de valores e a verificação das
saídas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 2  // número de threads (além do main)
#define NELEMS 10000  // número de entradas do vetor

#define DEBUG

typedef struct
{
    int thr_n;  // número da thread
    int* vector;  // o array, em sua totalidade
    // (ponteiro p/ sua posição inicial)
} thr_args;

void* square_elems(void* arg)
/* Recebe um ponteiro para uma struct do tipo thr_args e eleva ao quadrado parte das entradas do vetor apontado por seu campo vector,
de acordo com o número da thread thr_n. */
{
    thr_args* args = (thr_args*) arg;

    // Quantidade de elementos pelos quais a thread é responsável:
    int nelems = (NELEMS / NTHREADS);

    if ((NELEMS % NTHREADS) > args->thr_n)
    {
        nelems++;
    }

    for (int j=0; j < nelems; j++)
    {
        int pos = j * NTHREADS + args->thr_n;

        #ifdef DEBUG
            printf("Thread %d squaring position %d: ", args->thr_n, pos);
            printf("%d**2 = ", (args->vector)[pos]);
        #endif

        (args->vector)[pos] *= (args->vector)[pos];

        #ifdef DEBUG
            printf("%d\n", (args->vector)[pos]);
        #endif
        
    }

    free(arg);

    pthread_exit(NULL);
}

int main()
{
    #ifdef DEBUG
        printf("Beginning execution\n");
    #endif

    // Lendo o vetor de entrada:
    int input_vector[NELEMS];
    int vector_copy[NELEMS];
    // (Era pedido que testássemos a saída, então estou guardando uma
    // cópia para comparar com a saída de um procedimento single-threaded.)

    int v_pos = 0;  // para iterar sobre o vetor na inserção

    char* input_line = NULL;
	size_t len = 0;
	/* getline (abaixo) não vai usar &len, como eu não aloquei input_line, mas precisa receber ssize_t*, então foi necessário 
    declarar uma variável */

	ssize_t chars_read = getline(&input_line, &len, stdin);

	while(chars_read > 0) {
		int new_elem = atoi(input_line);
		// atoi já remove o (\r)\n

        #ifdef DEBUG
            printf("Read %d\n", new_elem);
        #endif
   

        input_vector[v_pos] = new_elem;
        vector_copy[v_pos] = new_elem;
        v_pos++;

		chars_read = getline(&input_line, &len, stdin);
	}  // término da leitura


    #ifdef DEBUG
        printf("Vector to square: \n");

        for (int i=0; i < NELEMS; i++)
        {
            printf("%d ", input_vector[i]);
        }

        printf("\n");
    #endif

    pthread_t tid_sistema[NTHREADS];

    thr_args* arg;

    // Criando as threads:
    for (int thread=0; thread<NTHREADS; thread++)
    {
        arg = malloc(sizeof(thr_args));

        if (arg == NULL)
        {
            printf("Falha na alocacao\n");
            exit(1);
        }

        arg->thr_n = thread;
        arg->vector = input_vector;

        #ifdef DEBUG
            printf("Creating thread %d\n", thread);
        #endif

        if (pthread_create(&tid_sistema[thread], NULL, square_elems, (void*) arg))
        {
            printf("Erro na criacao da thread %d\n", thread);
        }
    }

    // Aguardando todas as threads:
    for (int thread=0; thread<NTHREADS; thread++)
    {
        if (pthread_join(tid_sistema[thread], NULL))
        {
            printf("Erro ao aguardar a thread %d\n", thread);
        }
    }

    // Imprimindo o resultado:
    for (int i=0; i < NELEMS; i++)
    {
        printf("%d ", input_vector[i]);
    }

    printf("\n");

    /* Comparando a saída com a de um procedimento sequencial,
    com apenas uma thread, para teste: */

    for (int i=0; i<NELEMS; i++)
    {
        vector_copy[i] *= vector_copy[i];

        if (vector_copy[i] != input_vector[i])
        {
            printf("Erro na entrada %d", i);
            exit(1);
        }
    }

    #ifdef DEBUG
        printf("Ending execution\n");
    #endif

    return 0;
}