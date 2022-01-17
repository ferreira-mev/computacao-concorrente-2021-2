/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5
*/

public class Adder implements Runnable {  // hiss
    private int id;
    private int sum;

    public Adder(int tid)
    {
        this.id = tid;
        this.sum = 0;
    }

    public void run()
    {
        System.out.printf("[thread %d] Iniciando execucao%n", id);

        int min; int max;

        int chunkSize = Main.vecLen / Main.nThreads;

        min = id * chunkSize;

        if (id < Main.nThreads - 1)
        {
            max = (id + 1) * chunkSize;
        }
        else
        {
            max = Main.vecLen;
        }

        for (int i = min; i < max; i++)
        {
            sum += Main.vector.get(i);
        }

        System.out.printf("[thread %d] Soma das posicoes de ", id);
        System.out.printf("%d a %d: %d%n", min, max - 1, sum);

        Main.vector.addToSum(sum);

        System.out.printf("[thread %d] Encerrando execucao%n", id);
    }    
}
