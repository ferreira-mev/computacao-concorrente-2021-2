/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Classe com o método a ser executado pelas threads leitoras. Código 
adaptado do fornecido pela professora.
*/

public class Leitor implements Runnable {
    int id; //identificador da thread
    Monitor monitor;//objeto monitor para coordenar a lógica de execução das threads
  
    // Construtor
    public Leitor (int id, Monitor m) {
      this.id = id;
      this.monitor = m;
    }
  
    // Método executado pela thread
    public void run() {
    //   try {
    for (int iter = 0; iter < Main.maxIter; iter++) {
        this.monitor.entraLeitor(this.id);
        
        int valor = this.monitor.getVarComum();

        System.out.printf("# [thread %d] Variavel compartilhada ", this.id);

        if ((valor % 2) == 0) {
        System.out.printf("par");
        } else {
        System.out.printf("impar");
        }

        System.out.printf(" de valor %d%n", valor);

        this.monitor.saiLeitor(this.id);
        // sleep(this.delay); 
    }
    //   } catch (InterruptedException e) { 
    //       return; 
    //     }
    }
  }