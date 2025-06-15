# Algoritmo de Escalonamento com Prioridades Dinâmicas e Aging
- Davi Ludvig,
- João Paulo Oliveira e
- Gibram Goulart.

## Introdução
Este documento reúne um apanhado de informações acerca da solução dos discentes autores para resolver o enunciado do trabalho 2 da disciplina INE5412.

### 1. Objetivo
O objetivo deste trabalho é implementar um sistema de memória virtual paginada sob demanda em espaço de usuário, compreendendo a mecânica do tratamento de faltas de página e avaliando o desempenho de diferentes algoritmos de substituição de páginas (FIFO, RANDOM e um algoritmo personalizado). A implementação foi baseada no projeto do Prof. Douglas Thain (Universidade de Notre Dame), adaptado para o ambiente proposto em aula e disponibilizado pelo professor Giovani Gracioli.

### 2. Ambiente Experimental

- **Máquina utilizada**:  
  - Sistema Operacional: XXXXXXXXXXX
  - Processador: XXXXXXXXXXX
  - Memória RAM: XX GB  
  - Compilador: XXXXXXXXXX
  - Make: XXXXXXXXXX 

- **Execução dos testes**:  
Para cada programa (alpha, beta, gamma, delta), os seguintes comandos podem ser executados com variações:

  ```bash
  ./build/virtmem 10 10 rand alpha
  ./build/virtmem 10 10 fifo beta
  ./build/virtmem 10 10 custom gamma
  ...
  ```

### 3 Arquitetura
#### 3.1 Estrutura Basica
O projeto foi dividido nas seguintes classes:

- `Page_Table`: Interface com a tabela de páginas.
- `Disk`: Simulação de um disco secundário.
- `Page_Replacement`: Gerencia o estado dos frames físicos (livres/ocupados) e realiza as substituições de páginas.
- `Program`: Classe responsável por gerenciar os programas utilizados nas execuções (alpha, beta, gamma e delta). Ela coordena o fluxo das instruções, realiza acessos à memória virtual e simula os diferentes padrões de uso de memória definidos para os testes.
#### 3.2 Funcionamento do Tratador de Page Faults:
##### Passos:
1. Detecta a falta de página.
2. Se for a primeira falta, inicializa os frames.
3. Verifica por um frame livre:
   - Se disponível, lê a página do disco.
   - Se não, aplica o algoritmo de substituição:
     - Se a página vítima foi modificada, escreve-a no disco.
     - Libera o frame e invalida a entrada anterior.
4. Carrega a nova página no frame.
5. Atualiza a entrada na tabela de páginas.
6. Caso seja FIFO, adiciona à fila.
7. Se já estiver na memória mas com permissão insuficiente, ativa o bit de escrita.
> Estatísticas de execução (page faults, disk reads/writes) são coletadas automaticamente.

![Diagrama Fluxo](relatorio/fluxo.png)

### 4. Algoritmos de Substituição de Página

#### 4.1. RANDOM

Seleciona aleatoriamente uma página para substituição.

#### 4.2. FIFO (First-In, First-Out)

Mantém uma fila das páginas carregadas. A página mais antiga (a que entrou primeiro) é removida.


#### 4.3 CUSTOM

- Como algoritmo personalizado `custom` construído pelo grupo, foi decidido implementar uma versão do algoritmo de substituição de páginas LRU (Least Recently Used).
- Na inicialização da classe `Page_Replacement`, são definidos dois atributos:
1. O vetor de inteiros `tempo_acesso`, que armazena o tempo do último acesso de cada frame.
2. O inteiro `tempo_atual`, que é incrementado a cada vez que uma página é acessada, servindo como um contador de tempo global.
- Dentro do método `Page_Replacement::select_frame_to_be_removed`, é feita a checagem do algoritmo. Se for custom, aplica os seguintes passos:
1. Define o tempo mínimo como o maior inteiro possível.
2. Define o frame a ser removido como -1. (Nenhum frame)
3. Para cada frame, faça
   1. Se o tempo de acesso do frame for menor que o tempo mínimo, defina que o frame a ser removido é o atual.
4. Retorne o frame a ser removido.

#### 4.3.1 Comparações
##### Rand
- O algoritmo `rand` seleciona um frame aleatoriamente para remoção, o que pode levar a uma alta taxa de faltas de página, especialmente em cenários onde as páginas acessadas recentemente são necessárias novamente.
- Nesse cenário, exisitirão diversas possibilidades do `custom` se sair melhor que o `rand`, pois o `custom` prioriza a remoção de páginas que não foram acessadas recentemente, enquanto o `rand` não considera o histórico de acesso.

##### FIFO
- O algoritmo `fifo` remove a página que foi carregada há mais tempo, o que pode levar a uma taxa de faltas de página alta se as páginas acessadas recentemente forem removidas.
- O `custom` pode se sair melhor que o `fifo` em cenários onde as páginas acessadas recentemente são necessárias novamente, pois o `custom` prioriza a remoção de páginas que não foram acessadas recentemente.
- Além disso, em cenários onde há uma alta taxa de acesso a páginas, o `custom` pode se sair melhor que o `fifo`, pois o `fifo` não considera o histórico de acesso das páginas, enquanto o `custom` prioriza a remoção de páginas que não foram acessadas recentemente. Isso pode ser perceptível também em cenários com bastante leitura.

##### Gráfico para entendimento
- Foi constrúido o seguinte diagrama gráfico para melhor entendimento do algoritmo `custom`:
![Diagrama Custom](relatorio/custom.png)

Primeiro, alocou-se ambos vetores que representam o `frame` e o `tempo_acesso` para cada frame (`n`). O vetor de frames inicia com todos os valores iguais a -1, indicando que nenhum frame está ocupado. O vetor de `tempo_acesso` inicia com todos os valores iguais a 0, indicando que nenhum frame foi acessado.

Em seguida, são requisitadas as páginas `0`, `1`, `2`, ..., `n-1`, que ocupam todos os frames disponíveis. A cada requisição, o tempo atual é incrementado e o tempo de acesso do frame correspondente é atualizado para o valor do tempo atual.

Após isso, a página `1` é requisitada novamente, o que atualiza o tempo de acesso do frame correspondente para o valor do tempo atual (`n+1`).

Depois da requisição da página `1`, as páginas `n` e `n+1` são requisitadas. Como não há frames disponíveis, o algoritmo `custom` seleciona o frame com o menor tempo de acesso, que é o frame da página `0`, e o substitui pela página `n`. E o frame que alocava a página `2` é substituído pela página `n+1`, pois o tempo de acesso do frame da página `2` é o menor dentre os disponíveis no momento.

Com isso, observamos que o algoritmo `custom` (LRU) remove sempre a página que está há mais tempo sem ser acessada, o que pode levar a uma taxa de faltas de página menor em cenários onde as páginas acessadas recentemente são necessárias novamente.


### 5. Resultados e Análises
| Programa | Algoritmo | Page Faults | Disk Reads  | Disk Writes|
|----------|-----------|-------------|-------------|------------|
| 1        | 5         | 1           | 2           | 2         |
| 2        | 4         | 1           | 1           | 2          |
| 3        | 6         | 1           | 3           | 2          |
| 4        | 3         | 1           | 1           | 2          | 
| 5        | 7         | 1           | 2           | 2          |
| 6        | 4         | 1           | 1           | 2          |


