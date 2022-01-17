/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5
*/

public class Adder implements Runnable {  // hiss
    private int id;

    public Adder(int tid)
    {
        this.id = tid;
    }

    public void run()
    {
        System.out.println("thread " + this.id);
    }
}
