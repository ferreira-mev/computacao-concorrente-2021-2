/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5

Programa principal referente à atividade.
*/

public class Main
{
    static int nThreads;
    static int vecLen;

    static Shared vector;;
    public static void main (String[] args)
    {
        System.out.println("[main] Iniciando execucao");

        if (args.length != 2)
        {
            System.out.println("[main] Modo de uso:");
            System.out.println("java Main n l");
            System.out.printf("[main] onde n eh o numero de threads ");
            System.out.printf("e l eh o tamanho do vetor%n");
            return;
        }  // else implícito

        try
        {
            nThreads = Integer.parseInt(args[0]);
            vecLen = Integer.parseInt(args[1]);   
        }
        catch (NumberFormatException e)
        {
            System.err.printf("[main] Erro: os argumentos devem ser inteiros");
            return;
        }

        vector = new Shared(vecLen);

        // Criando e "disparando" as threads:

        Thread[] threads = new Thread[nThreads];

        for (int i=0; i < nThreads; i++)
        {
            threads[i] = new Thread(new Adder(i));
        }

        for (int i=0; i < nThreads; i++) { threads[i].start(); }

        // Sincronizando:
        for (int i=0; i < nThreads; i++)
        {
            try { threads[i].join(); }
            catch (InterruptedException e)
            {
                System.err.println("[main] Falha na sincronizacao");
                return;
            }
        }

        // Verificando a corretude:

        int total = vector.getSum();
        int expected = (vecLen * (vecLen + 1)) / 2;

        if (total != expected)
        {
            System.err.printf("[main] Erro no calculo - valor ");
            System.err.printf("esperado: %d; obtido: %d%n", expected, total);
            return;
        }

        System.out.printf("[main] Soma das entradas (i.e., dos ");
        System.out.printf("naturais de 1 a %d): %d%n", vecLen, total);
        System.out.println("[main] Encerrando execucao com sucesso");
    }
}
