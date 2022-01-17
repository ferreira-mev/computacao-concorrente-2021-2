/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5
*/

public class Main
{
    static int nThreads = 2;  // apenas para testar, por enquanto
    static int vecLen = 100;  // idem

    static Shared vector = new Shared(vecLen);
    public static void main (String[] args)
    {
        System.out.println("[main] Iniciando execucao");

        Thread[] threads = new Thread[nThreads];
        int total = 0;

        for (int i=0; i < nThreads; i++)
        {
            threads[i] = new Thread(new Adder(i));
        }

        for (int i=0; i < nThreads; i++) { threads[i].start(); }

        for (int i=0; i < nThreads; i++)
        {
            try
            {
                threads[i].join();
                total += threads[i].getSum();
            }
            catch (InterruptedException e)
            {
                System.err.println("[main] Falha na sincronizacao");
                return;
            }
        }

        // Verificando a corretude:

        int expected = (vecLen * (vecLen + 1)) / 2;

        if (total != expected)
        {
            System.err.printf("[main] Erro no calculo - valor ");
            System.err.printf("esperado: %d; obtido: %d", expected, total);
        }

        System.out.printf("[main] Soma das entradas (i.e., dos ");
        System.out.printf("naturais de 1 a %d): %d", vecLen, total);
        System.out.println("[main] Encerrando execucao com sucesso");
    }
}
