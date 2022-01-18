/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Monitor que implementa a lógica do padrão leitores/escritores. Código
 adaptado do fornecido pela professora.
*/

public class Monitor {
    private int leit, escr, varComum;
    
    // Construtor
    Monitor() { 
       this.leit = 0; //leitores lendo (0 ou mais)
       this.escr = 0; //escritor escrevendo (0 ou 1)
       this.varComum = 0;
    } 
    
    // Entrada para leitores
    public synchronized void entraLeitor (int id) {
      try { 
        while (this.escr > 0) {
           System.out.println ("le.leitorBloqueado("+id+")");
           wait();  //bloqueia pela condicao logica da aplicacao 
        }
        this.leit++;  //registra que ha mais um leitor lendo
        System.out.println ("le.leitorLendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para leitores
    public synchronized void saiLeitor (int id) {
       this.leit--; //registra que um leitor saiu
       if (this.leit == 0) 
             this.notify(); //libera escritor (caso exista escritor bloqueado)
       System.out.println ("le.leitorSaindo("+id+")");
    }
    
    // Entrada para escritores
    public synchronized void entraEscritor (int id) {
      try { 
        while ((this.leit > 0) || (this.escr > 0)) {
           System.out.println ("le.escritorBloqueado("+id+")");
           wait();  //bloqueia pela condicao logica da aplicacao 
        }
        this.escr++; //registra que ha um escritor escrevendo
        System.out.println ("le.escritorEscrevendo("+id+")");
      } catch (InterruptedException e) { }
    }
    
    // Saida para escritores
    public synchronized void saiEscritor (int id) {
       this.escr--; //registra que o escritor saiu
       notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
       System.out.println ("le.escritorSaindo("+id+")");
    }

    // O getter e o setter, neste caso, servem não só para 
    // encapsulamento como tb para assegurar exclusão mútua
    // no acesso à variável compartilhada
    public synchronized int getVarComum () {
        return this.varComum;
    }

    public synchronized void setVarComum (int valor) {
        this.varComum = valor;
    }

    public synchronized void incrVarComum() {
        this.varComum++;

        // (para evitar o acesso por outra thread entre
        // um get seguido de set para essa finalidade)
    }
}