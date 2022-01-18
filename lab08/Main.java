/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 8 -- Atividade 1

Programa principal. Código adaptado do fornecido pela professora.
*/

public class Main {
    static int nLeitoras;
    static int nEscritoras;
    static int nLeitEscr;
    static int sleepDelay;
    static int maxIter;
    // acrescentei um máximo de iterações a serem executadas por cada
    // thread só para não ter um programa que roda indefinidamente
  
    public static void main (String[] args) {
        if ((args.length != 3) && (args.length != 5)) {
            System.out.println("# [main] Modo de uso - opcao 1:");
            System.out.println("# java Main l e le");
            System.out.println("# (sem o simbolo \"#\")");
            System.out.printf("# onde l eh o numero de threads ");
            System.out.println("# leitoras, e de escritoras e");
            System.out.println("# le de leitoras-escritoras");
            System.out.println();
            System.out.println("# [main] Modo de uso - opcao 2:");
            System.out.println("# java Main l e le s m");
            System.out.println("# (sem o simbolo \"#\")");
            System.out.printf("# onde l eh o numero de threads ");
            System.out.println("leitoras, e de escritoras e");
            System.out.printf("# le de leitoras-escritoras; s eh ");
            System.out.println("o tempo de espera entre iteracoes");
            System.out.println("# em ms e m eh o numero max de iteracoes");

            return;
        }

        nLeitoras = Integer.parseInt(args[0]);
        nEscritoras = Integer.parseInt(args[1]);
        nLeitEscr = Integer.parseInt(args[2]);

        if (args.length == 5) {
            sleepDelay = Integer.parseInt(args[3]);
            maxIter = Integer.parseInt(args[4]);
        } else {
            sleepDelay = 5000;
            maxIter = 40;
        }

        int i;
        Monitor monitor = new Monitor(); // Monitor (objeto compartilhado entre leitores e escritores)

        Thread[] leitoras = new Thread[nLeitoras];
        Thread[] escritoras = new Thread[nEscritoras];
        Thread[] leitEscr = new Thread[nLeitEscr];
  
        //inicia o log de saida
        System.out.println("import verificaLE");
        System.out.println("le = verificaLE.LE()");

        // Criando as threads:
        for (i=0; i<nLeitoras; i++) {
            leitoras[i] = new Thread(new Leitor(i+1, monitor));
        }

        for (i=0; i<nEscritoras; i++) {
            escritoras[i] = new Thread(new Escritor(i+1, monitor));
        }

        int maxId = Math.max(nLeitoras, nEscritoras);
        // como agora há mais de um tipo de thread capaz de ler ou
        // escrever, repetir IDs seria ambíguo para fins de debug

        for (i=0; i<nLeitEscr; i++) {
            leitEscr[i] = new Thread(new LeitorEscritor(maxId+i+1, monitor));
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