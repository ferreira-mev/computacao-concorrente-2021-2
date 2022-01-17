/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5
*/

public class Main
{
    static int nThreads = 2;  // apenas para testar, por enquanto
    public static void main (String[] args)
    {
        // System.out.println("hello world");

        Thread[] threads = new Thread[nThreads];

        for (int i=0; i < nThreads; i++)
        {
            threads[i] = new Adder(i);
        }

        for (int i=0; i < nThreads; i++) { threads[i].start(); }

        for (int i=0; i < nThreads; i++)
        {
            try { threads[i].join(); }
            catch (InterruptedException e)
            {
                System.err.println("Falha na sincronizacao");
                return;
            }
        }

        System.out.println("main terminou");
    }
}
