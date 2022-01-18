/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Classe com o método a ser executado pelas threads escritoras. Código 
adaptado do fornecido pela professora.
*/

public class Escritor implements Runnable {
    int id; //identificador da thread
    Monitor monitor; //objeto monitor para coordenar a lógica de execução das threads
  
    // Construtor
    public Escritor(int id, Monitor m) {
        this.id = id;
        this.monitor = m;
    }
  
    // Método executado pela thread
    public void run() {
        try {
            for (int iter = 0; iter < Main.maxIter; iter++) {
                this.monitor.entraEscritor(this.id);
                this.monitor.setVarComum(this.id);
                this.monitor.saiEscritor(this.id); 
                sleep(Main.sleepDelay);
            }
        } catch (InterruptedException e) { return; }
    }
  }