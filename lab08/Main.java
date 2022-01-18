/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Programa principal. Código adaptado do fornecido pela professora.
*/

public class Main {
    static final int nLeitoras = 4;
    static final int nEscritoras = 10;
    static final int nLeitEscr = 1;
    static final int sleepDelay = 500;
    static final int maxIter = 5;
    // acrescentei um máximo de iterações a serem executadas por cada
    // thread só para não ter um programa que roda indefinidamente
  
    public static void main (String[] args) {
        int i;
        Monitor monitor = new Monitor();            // Monitor (objeto compartilhado entre leitores e escritores)

        Thread[] leitoras = new Thread[nLeitoras];
        Thread[] escritoras = new Thread[nEscritoras];
        Thread[] leitEscr = new Thread[nLeitEscr];
  
        //inicia o log de saida
        System.out.println("import verificaLE");
        System.out.println("le = verificaLE.LE()");

        // Criando as threads:
        for (i=0; i<nLeitoras; i++) {
            leitoras[i] = new Thread(new Leitor(i+1, monitor));
        //  leitoras[i].start(); 
        }

        for (i=0; i<nEscritoras; i++) {
            escritoras[i] = new Thread(new Escritor(i+1, monitor));
        //  escritoras[i].start(); 
        }

        int maxId = Math.max(nLeitoras, nEscritoras);
        // como agora há mais de um tipo de thread capaz de ler ou
        // escrever, repetir IDs seria ambíguo

        for (i=0; i<nLeitEscr; i++) {
            leitEscr[i] = new Thread(new LeitorEscritor(maxId+i+1, monitor));
        //  leitEscr[i].start(); 
        }

        // Inicializando as threads:

        // (tentei evitar inicializar um grupo de cada vez,
        // pois, com poucas iterações, um acaba terminando antes)

        int maxN = Math.max(maxId, nLeitEscr);

        for (i=0; i<maxN; i++) {
            if (i < nLeitoras) { leitoras[i].start(); } 
            if (i < nEscritoras) { escritoras[i].start(); }
            if (i < nLeitEscr) { leitEscr[i].start(); }
        }
    }
  }