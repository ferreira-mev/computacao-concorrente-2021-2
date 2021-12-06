# Resultados das medições de ganho de desempenho

Eu já tenho um arquivo README na raiz do repositório, então estou usando um nome diferente do que estava nas instruções do trabalho, mas que acredito ser óbvio o suficiente (até por não haver tantos arquivos neste subdiretório).

Mais informações sobre o processador utilizado estão no relatório do laboratório 2, conforme havia sido solicitado, mas é um Intel Core i7-7700HQ com 4 cores físicos, elevados a 8 cores virtuais por meio de hyperthreading. Assim como no laboratório 2, decidi efetuar as medidas também com 8 threads, por essa razão.

## Medições de T_sequencial/T_concorrente, em segundos

Estou incluindo as saídas de duas execuções diferentes, a título de comparação.

**len**|**1 thread**|**2 threads**|**4 threads**|**8 threads**
:-----:|:-----:|:-----:|:-----:|:-----:
100000|0.732388|1.127772|1.555416|1.106179
10000000|0.924943|1.742306|3.591551|4.021165
100000000|0.903273|1.810215|3.649224|4.149196

**len**|**1 thread**|**2 threads**|**4 threads**|**8 threads**
:-----:|:-----:|:-----:|:-----:|:-----:
100000|0.913199|1.647287|2.709274|2.298533
10000000|0.964732|1.851598|3.748633|5.871734
100000000|0.950832|1.890296|3.841666|5.749959

Acrescento alguns comentários. Diferentemente do laboratório 2 e conforme o esperado, desta vez, sim, observei uma redução de desempenho ao executar o algoritmo concorrente com apenas uma thread; isso ocorre por haver um overhead do processo de paralelização, com a criação e sincronização de threads, sem um ganho por de fato executar o algoritmo em múltiplas instâncias simultâneas.

Acredito que tenha obtido esse resultado porque, pensando no porquê de isso não ter ocorrido no laboratório 2, eu me esforcei para fazer medidas mais precisas desta vez. Em particular, tentei ser mais cuidadosa em de fato incluir na medição de tempo concorrente _todas_ as etapas executadas em função da paralelização (exceto pela leitura do número de threads na entrada); além disso, busquei tornar mais próximos os algoritmos sequencial e concorrente em seus demais aspectos, acessando diretamente as variáveis globais também na função sequencial, para evitar eventuais alterações no tempo de execução devido à diferença na forma de acesso às variáveis utilizadas (variáveis globais vs parâmetros por referência vs parâmetros por valor).

Outra diferença em relação ao laboratório 2 foi que, agora, para os comprimentos maiores, usar 8 threads foi mais eficiente do que usar 4. Suponho que isso possa ter ocorrido porque, no laboratório anterior, deixei cada thread responsável por linhas alternadas, ao passo que desta vez, similarmente ao que foi mostrado nas aulas da terceira semana, aloquei para cada thread um bloco de elementos contíguos. Como meu processador é capaz de gerar dois cores virtuais a partir de cada core físico, esse acesso a posições distantes de memória por duas threads que estão "dividindo" o mesmo processador talvez tenha incorrido num gasto de tempo grande demais para que essa divisão compensasse. Entretanto, como meu entendimento sobre isso é limitado, pode ter sido apenas coincidência, e isso pode ter ocorrido por outras razões, como diferenças entre os próprios algoritmos -- no lab 2, por exemplo, cada thread escrevia suas saídas numa matriz de resultados à medida que calculava, ao passo que, aqui, cada uma mantém apenas um contador interno que é retornado ao final.

Finalmente, em linha com a expectativa, o ganho de performance no caso do vetor menor, de comprimento 10^5, não foi tão expressivo quanto nos demais casos. Ao executar casos de teste com vetores realmente pequenos, da ordem de 10^1, observei, inclusive, uma execução mais lenta com qualquer quantidade de threads.