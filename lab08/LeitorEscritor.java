/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Classe com o método a ser executado pelas threads leitoras e 
escritoras. Código adaptado do fornecido pela professora.
*/

public class LeitorEscritor implements Runnable {
    int id; //identificador da thread
    Monitor monitor;//objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    public LeitorEscritor (int id, Monitor m) {
        this.id = id;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        double j=777777777.7, i;
        try {
            for (int iter = 0; iter < Main.maxIter; iter++) {
                this.monitor.entraLeitor(this.id);

                int valor = monitor.getVarComum();

                System.out.println("# [thread " + this.id + 
                    "] Variavel compartilhada com valor " + valor);

                this.monitor.saiLeitor(this.id);

                for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para simbolizar o tempo de processamento

                this.monitor.entraEscritor(this.id);
                this.monitor.incrVarComum();
                this.monitor.saiEscritor(this.id);

                Thread.sleep(Main.sleepDelay);
            }
        } catch (InterruptedException e) { return; }
    }
}