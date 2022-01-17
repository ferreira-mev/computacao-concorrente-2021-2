/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5
*/

public class Main
{
    static int nThreads = 2;  // apenas para testar, por enquanto
    static int vecLen = 100;  // idem
    public static void main (String[] args)
    {
        Shared vector = new Shared(vecLen);
        Thread[] threads = new Thread[nThreads];

        for (int i=0; i < nThreads; i++)
        {
            threads[i] = new Thread(new Adder(i));
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
