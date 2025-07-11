# A-Rusty-Screw

Este é um jogo de plataforma 2D desenvolvido em C++ com o aux�lio da biblioteca SDL2. O jogador controla um personagem que pode pular, atacar e interagir com o ambiente para progredir através dos níveis.

## Visão Geral do Projeto

O objetivo do jogo é navegar por diferentes níveis, superando obstáculos, derrotando inimigos e resolvendo quebra-cabeças simples. O personagem principal possui duas formas de ataque distintas: um ataque cortante e um ataque perfurante. Cada tipo de ataque é eficaz contra um tipo específico de "parafuso" que pode ser encontrado nos níveis, incentivando o jogador a alternar entre as ferramentas para progredir. Além disso, o jogador conta com a ajuda de uma galinha de estimação chamada Anselma.

## Funcionalidades Principais

  * **Movimentação de Personagem:** O jogador pode se mover para a esquerda e para a direita, pular e realizar pulos na parede.
  * **Sistema de Combate:** O jogador pode alternar entre dois tipos de ataque (cortante e perfurante) para destruir obstáculos e derrotar inimigos.
  * **Inimigos:** O jogo possui inimigos que podem causar dano ao jogador e serem derrotados.
  * **Física e Colisões:** O jogo implementa um sistema de física simples com gravidade e detecção de colisão com plataformas, plataformas sólidas e caixotes.
  * **Níveis:** Os níveis são carregados a partir de arquivos TMX, permitindo um design de nível flexível.
  * **Câmera:** A câmera segue o jogador suavemente, com margens para uma melhor experiência de visualização.
  * **HUD (Heads-Up Display):** Exibe informações importantes para o jogador, como o tipo de ataque selecionado e os controles do jogo.
  * **Efeitos Visuais:** O jogo inclui um efeito de CRT (scanlines e vinheta) para uma estética retrô.
  * **Parallax Scrolling:** O fundo se move em diferentes velocidades para criar uma ilusão de profundidade.

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

  * **`assets/`**: Contém todos os recursos do jogo, como sprites, fontes e mapas.
  * **`bin/`**: Contém os arquivos executáveis do projeto.
  * **`include/arscrew/`**: Contém os arquivos de cabeçalho (`.h`) para as classes do jogo.
  * **`my-lib-master/`**: Uma biblioteca de apoio com várias funcionalidades, incluindo matemática (vetores, matrizes), gerenciamento de memória e sistemas de eventos.
  * **`src/`**: Contém os arquivos de implementaçãoo (`.cpp`) para as classes do jogo.
  * **`Makefile`**: (Não fornecido) Arquivo de compilação para o projeto.
  * **`run.sh`**: Script para executar o jogo.

## Dependências

  * **SDL2:** Biblioteca multimídia para gráficos, som e entrada.
  * **SDL2\_image:** Biblioteca para carregar vários formatos de imagem.
  * **SDL2\_ttf:** Biblioteca para renderizar fontes TrueType.
  * **SDL2\_mixer:** Biblioteca para áudio.
  * **tinyxml2:** Biblioteca para parsing de arquivos XML (usada para os mapas TMX).

## Como Compilar e Executar

1.  **Instale as dependências:**
    Certifique-se de que as bibliotecas SDL2, SDL2\_image, SDL2\_ttf e SDL2\_mixer estão instaladas em seu sistema.

2.  **Compile o projeto:**
    (Instruções de compilação seriam fornecidas aqui se o `Makefile` estivesse disponível).

3.  **Execute o jogo:**
    Use o script `run.sh` para iniciar o jogo:

    ```bash
    ./run.sh
    ```

## Detalhes de Implementação

### Gerenciamento do Jogo

A classe `GameManager` é o coração do jogo, responsável por:

  * Inicializar o SDL e os subsistemas de áudio e fontes.
  * Gerenciar o loop principal do jogo (eventos, atualização, renderização).
  * Controlar as transições entre os níveis.
  * Gerenciar a renderização do fundo com efeito parallax.
  * Atualizar e renderizar a HUD.

### Mundo do Jogo

A classe `GameWorld` é responsável por:

  * Carregar e gerenciar os objetos do nível a partir de um arquivo TMX.
  * Manter listas de todos os objetos do jogo (plataformas, inimigos, etc.).
  * Atualizar o estado de todos os objetos do jogo.
  * Renderizar todos os objetos do mundo.

### Personagem

A classe `Player` implementa a l�gica do personagem principal, incluindo:

  * Movimentação, pulo e pulo na parede.
  * Sistema de ataque com troca de ferramentas.
  * Sistema de vida e dano com invulnerabilidade temporária.
  * Animações para diferentes estados (parado, correndo, pulando, atacando).

### Colisões

A classe `CollisionEngine` é uma classe que lida com toda a lógica de colisão entre os objetos do jogo, incluindo:

  * Colisão entre objetos dinâmicos e estáticos.
  * Colisões com diferentes tipos de plataformas.
  * Interações específicas do jogador, como colisão com portas e caixotes.