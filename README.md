# A-Rusty-Screw

Este � um jogo de plataforma 2D desenvolvido em C++ com o aux�lio da biblioteca SDL2. O jogador controla um personagem que pode pular, atacar e interagir com o ambiente para progredir atrav�s dos n�veis.

## Vis�o Geral do Projeto

O objetivo do jogo � navegar por diferentes n�veis, superando obst�culos, derrotando inimigos e resolvendo quebra-cabe�as simples. O personagem principal possui duas formas de ataque distintas: um ataque cortante e um ataque perfurante. Cada tipo de ataque � eficaz contra um tipo espec�fico de "parafuso" que pode ser encontrado nos n�veis, incentivando o jogador a alternar entre as ferramentas para progredir. Al�m disso, o jogador conta com a ajuda de uma galinha de estima��o chamada Anselma.

## Funcionalidades Principais

  * **Movimenta��o de Personagem:** O jogador pode se mover para a esquerda e para a direita, pular e realizar pulos na parede.
  * **Sistema de Combate:** O jogador pode alternar entre dois tipos de ataque (cortante e perfurante) para destruir obst�culos e derrotar inimigos.
  * **Inimigos:** O jogo possui inimigos que podem causar dano ao jogador e serem derrotados.
  * **F�sica e Colis�es:** O jogo implementa um sistema de f�sica simples com gravidade e detec��o de colis�o com plataformas, plataformas s�lidas, rampas e caixotes.
  * **N�veis:** Os n�veis s�o carregados a partir de arquivos TMX, permitindo um design de n�vel flex�vel.
  * **C�mera:** A c�mera segue o jogador suavemente, com margens para uma melhor experi�ncia de visualiza��o.
  * **HUD (Heads-Up Display):** Exibe informa��es importantes para o jogador, como o tipo de ataque selecionado e os controles do jogo.
  * **Efeitos Visuais:** O jogo inclui um efeito de CRT (scanlines e vinheta) para uma est�tica retr�.
  * **Parallax Scrolling:** O fundo se move em diferentes velocidades para criar uma ilus�o de profundidade.

## Estrutura do Projeto

O projeto est� organizado da seguinte forma:

  * **`assets/`**: Cont�m todos os recursos do jogo, como sprites, fontes e mapas.
  * **`bin/`**: Cont�m os arquivos execut�veis do projeto.
  * **`include/arscrew/`**: Cont�m os arquivos de cabe�alho (`.h`) para as classes do jogo.
  * **`my-lib-master/`**: Uma biblioteca de apoio com v�rias funcionalidades, incluindo matem�tica (vetores, matrizes), gerenciamento de mem�ria e sistemas de eventos.
  * **`src/`**: Cont�m os arquivos de implementa��o (`.cpp`) para as classes do jogo.
  * **`Makefile`**: (N�o fornecido) Arquivo de compila��o para o projeto.
  * **`run.sh`**: Script para executar o jogo.

## Depend�ncias

  * **SDL2:** Biblioteca multim�dia para gr�ficos, som e entrada.
  * **SDL2\_image:** Biblioteca para carregar v�rios formatos de imagem.
  * **SDL2\_ttf:** Biblioteca para renderizar fontes TrueType.
  * **SDL2\_mixer:** Biblioteca para �udio.
  * **tinyxml2:** Biblioteca para parsing de arquivos XML (usada para os mapas TMX).

## Como Compilar e Executar

1.  **Instale as depend�ncias:**
    Certifique-se de que as bibliotecas SDL2, SDL2\_image, SDL2\_ttf e SDL2\_mixer est�o instaladas em seu sistema.

2.  **Compile o projeto:**
    (Instru��es de compila��o seriam fornecidas aqui se o `Makefile` estivesse dispon�vel).

3.  **Execute o jogo:**
    Use o script `run.sh` para iniciar o jogo:

    ```bash
    ./run.sh
    ```

## Detalhes de Implementa��o

### Gerenciamento do Jogo

A classe `GameManager` � o cora��o do jogo, respons�vel por:

  * Inicializar o SDL e os subsistemas de �udio e fontes.
  * Gerenciar o loop principal do jogo (eventos, atualiza��o, renderiza��o).
  * Controlar as transi��es entre os n�veis.
  * Gerenciar a renderiza��o do fundo com efeito parallax.
  * Atualizar e renderizar a HUD.

### Mundo do Jogo

A classe `GameWorld` � respons�vel por:

  * Carregar e gerenciar os objetos do n�vel a partir de um arquivo TMX.
  * Manter listas de todos os objetos do jogo (plataformas, inimigos, etc.).
  * Atualizar o estado de todos os objetos do jogo.
  * Renderizar todos os objetos do mundo.

### Personagem

A classe `Player` implementa a l�gica do personagem principal, incluindo:

  * Movimenta��o, pulo e pulo na parede.
  * Sistema de ataque com troca de ferramentas.
  * Sistema de vida e dano com invulnerabilidade tempor�ria.
  * Anima��es para diferentes estados (parado, correndo, pulando, atacando).

### Colis�es

A classe `CollisionEngine` � uma classe est�tica que lida com toda a l�gica de colis�o entre os objetos do jogo, incluindo:

  * Colis�o entre objetos din�micos e est�ticos.
  * Colis�es com diferentes tipos de plataformas e rampas.
  * Intera��es espec�ficas do jogador, como colis�o com portas e caixotes.