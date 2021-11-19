**A1.3** Na prática, é impossível saber; conforme o esperado, elas são criadas sequencialmente pelo programa principal, mas o programa não identifica qual "Hello World" é proveniente de qual thread. Nota-se, porém, que variam os momentos de impressão dos "Hello Worlds" em relação às saídas do programa principal.

**A2.3** Desta vez, as saídas das threads eram identificadas. Eu esperaria que a ordem das saídas variasse, pois não estamos controlando a ordem nem de execução das threads, nem de impressão (forçosamente sequencial). Além disso, no programa similar ao da aula que fiz enquanto assistia, a ordem de fato variou. Neste caso, entretanto, observei um efeito curioso: obtive 0, 1, 2, ..., 9 todas as vezes, e apenas o momento de impressão dos "Hello World" variava.

**A3.3** 