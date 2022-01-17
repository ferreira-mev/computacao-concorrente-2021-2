/* Computação Concorrente -- profa. Silvana Rossetto -- 2021.2 -- UFRJ
Aluna: Eduarda Ferreira
Laboratório 7 -- Atividade 5

Recurso (vetor de inteiros) compartilhado entre as threads.
*/

public class Shared {
    private int[] vector;

    // private int sum;

    public Shared(int len)
    {
        // Inicializando o vetor com 1, 2, ..., len apenas para
        // simplificar a verificação da corretude:

        this.vector = new int[len];
    
        for (int i=0; i < len; i++)
        {
            vector[i] = i + 1;
        }
    }

    public int get(int i)
    {
        // não precisa de synchronized porque é somente leitura,
        // então não há problema em haver acessos simultâneos;
        // seria necessário caso houvesse um método que alterasse
        // os valores
        return this.vector[i];
    }
}
