# Análise Detalhada do Projeto "A-Rusty-Screw"

> Documento gerado em: 02/02/2026
> Branch analisada: `dev`

---

## Sumário

1. [Visão Geral](#1-visão-geral)
2. [Estrutura do Projeto](#2-estrutura-do-projeto)
3. [Tecnologias Utilizadas](#3-tecnologias-utilizadas)
4. [Arquitetura do Projeto](#4-arquitetura-do-projeto)
5. [Componentes Principais](#5-componentes-principais)
6. [Sistema de Build](#6-sistema-de-build)
7. [Dependências Externas](#7-dependências-externas)
8. [Sistema de Assets](#8-sistema-de-assets)
9. [Sistema de Mapas (Níveis)](#9-sistema-de-mapas-níveis)
10. [Fluxo do Jogo](#10-fluxo-do-jogo)
11. [Mecânicas do Jogo](#11-mecânicas-do-jogo)
12. [Funcionalidades Especiais](#12-funcionalidades-especiais)
13. [Padrões de Design](#13-padrões-de-design)
14. [Código-Chave](#14-código-chave)
15. [Estado Atual do Projeto](#15-estado-atual-do-projeto)
16. [Conclusões](#16-conclusões)

---

## 1. Visão Geral

**"A Rusty Screw"** é um jogo de plataforma 2D desenvolvido em C++20 utilizando a biblioteca SDL2. O jogo apresenta mecânicas de combate baseadas em diferentes tipos de ferramentas, sistema de progressão por níveis, um boss final, e estética visual retrô inspirada em jogos 8-bit.

### Características Principais

- Jogo de plataforma side-scrolling
- Sistema de ataque dual (CUTTING e PIERCING)
- Progressão por 7 níveis + arena de boss
- Pet companion (Anselma, a galinha)
- Sistema de high scores persistente
- Efeitos visuais CRT/retrô
- Suporte a gamepad

### Estatísticas do Código

| Métrica | Valor |
|---------|-------|
| Total de arquivos de código | 59 arquivos (.h e .cpp) |
| Total de linhas de código | ~10.489 linhas |
| Arquivos header | 28 arquivos |
| Arquivos de implementação | 31 arquivos |
| Mapas/Níveis | 17 arquivos TMX |

---

## 2. Estrutura do Projeto

```
/home/netorapg/projects/A-Rusty-Screw/
│
├── assets/                    # Recursos do jogo
│   ├── fonts/                 # Fontes TrueType
│   │   ├── Arial.ttf
│   │   ├── All Star Resort.ttf
│   │   ├── Megadeth.ttf
│   │   ├── METALORD.TTF
│   │   └── Type Machine.ttf
│   ├── parallax/              # Camadas de parallax (1-5)
│   │   ├── 1.png
│   │   ├── 2.png
│   │   ├── 3.png
│   │   ├── 4.png
│   │   └── 5.png
│   ├── [sprites diversos]     # Sprites de personagens e objetos
│   └── [áudio]                # Músicas e efeitos sonoros
│
├── bin/                       # Executáveis compilados
│   └── main                   # Executável principal
│
├── include/                   # Arquivos de cabeçalho
│   └── arscrew/
│       ├── Animation.h
│       ├── AnimationFrame.h
│       ├── Camera.h
│       ├── Chicken.h
│       ├── CollisionEngine.h
│       ├── Crate.h
│       ├── CreditsScreen.h
│       ├── Decoration.h
│       ├── Door.h
│       ├── DynamicObject.h
│       ├── Enemy.h
│       ├── GameManager.h
│       ├── GameOverScreen.h
│       ├── GameWorld.h
│       ├── Gate.h
│       ├── HighScoresManager.h
│       ├── HighScoresScreen.h
│       ├── HUD.h
│       ├── Object.h
│       ├── PauseMenu.h
│       ├── Platform.h
│       ├── Player.h
│       ├── Punktauro.h
│       ├── ScoreEntryScreen.h
│       ├── Screw.h
│       ├── SolidPlatform.h
│       ├── StartMenu.h
│       ├── StaticObject.h
│       └── ToolTip.h
│
├── src/                       # Implementações
│   ├── Animation.cpp
│   ├── AnimationFrame.cpp
│   ├── Camera.cpp
│   ├── Chicken.cpp
│   ├── CollisionEngine.cpp
│   ├── Crate.cpp
│   ├── CreditsScreen.cpp
│   ├── Decoration.cpp
│   ├── Door.cpp
│   ├── DynamicObject.cpp
│   ├── Enemy.cpp
│   ├── GameManager.cpp
│   ├── GameOverScreen.cpp
│   ├── GameWorld.cpp
│   ├── Gate.cpp
│   ├── HighScoresManager.cpp
│   ├── HighScoresScreen.cpp
│   ├── HUD.cpp
│   ├── main.cpp
│   ├── Object.cpp
│   ├── PauseMenu.cpp
│   ├── Platform.cpp
│   ├── Player.cpp
│   ├── Punktauro.cpp
│   ├── ScoreEntryScreen.cpp
│   ├── Screw.cpp
│   ├── SolidPlatform.cpp
│   ├── StartMenu.cpp
│   ├── StaticObject.cpp
│   └── ToolTip.cpp
│
├── map/                       # Arquivos de níveis (Tiled TMX)
│   ├── level1.tmx - level7.tmx
│   ├── bossarena.tmx
│   ├── demomap.tmx
│   ├── demoroom1-4.tmx, demoroom6.tmx
│   ├── gate_example.tmx
│   ├── hole.tmx
│   ├── mapabanca1-3.tmx
│   ├── tooltip_example.tmx
│   └── fulltile.tsx           # Definição do tileset
│
├── obj/                       # Arquivos objeto compilados
│
├── Makefile                   # Script de compilação
├── README.md                  # Documentação principal
├── class_diagram.md           # Diagrama UML de classes
├── diagrama.puml              # Diagrama PlantUML
├── run.sh                     # Script para executar o jogo
└── .gitignore                 # Arquivos ignorados pelo git
```

---

## 3. Tecnologias Utilizadas

### 3.1 Linguagem de Programação

| Tecnologia | Versão/Padrão | Descrição |
|------------|---------------|-----------|
| **C++** | C++20 | Linguagem principal do projeto |
| **g++** | Compatível C++20 | Compilador utilizado |

### 3.2 Bibliotecas SDL2

| Biblioteca | Propósito |
|------------|-----------|
| **SDL2** | Biblioteca multimídia base (gráficos, eventos, input) |
| **SDL2_image** | Carregamento de formatos de imagem (PNG, JPG) |
| **SDL2_ttf** | Renderização de fontes TrueType |
| **SDL2_mixer** | Sistema de áudio e música |

### 3.3 Outras Bibliotecas

| Biblioteca | Propósito |
|------------|-----------|
| **tinyxml2** | Parsing de arquivos XML (mapas TMX do Tiled) |
| **MyLib** | Biblioteca customizada com Vector2D e utilitários |

### 3.4 Ferramentas Externas

| Ferramenta | Propósito |
|------------|-----------|
| **Tiled Map Editor** | Criação e edição de níveis (formato TMX) |
| **Aseprite** | Criação de sprites (arquivos .ase encontrados) |

---

## 4. Arquitetura do Projeto

### 4.1 Padrão Arquitetural

O projeto segue um modelo **Component-Based Game Engine** com hierarquia de objetos bem definida.

### 4.2 Hierarquia de Classes de Objetos

```
Object (classe base abstrata)
│
├── StaticObject (objetos imóveis)
│   ├── Platform (plataformas normais - pode passar por baixo)
│   ├── SolidPlatform (plataformas sólidas - bloqueia em todas direções)
│   ├── Door (portas para transição de níveis)
│   ├── Decoration (decorações visuais sem colisão)
│   ├── Gate (portões que abrem/fecham vinculados a parafusos)
│   ├── ToolTip (pontas de ferramentas - coletáveis)
│   └── Screw (parafusos - alvos de destruição)
│
└── DynamicObject (objetos com física e movimento)
    ├── Player (personagem principal)
    ├── Enemy (inimigos genéricos)
    │   └── Punktauro (boss único do jogo - herda de Enemy)
    ├── Chicken (galinha de estimação "Anselma")
    └── Crate (caixotes que podem ser empurrados)
```

### 4.3 Diagrama de Gerenciamento

```
main.cpp
    │
    └── GameManager (coordena todo o jogo)
            │
            ├── GameWorld (gerencia todos os objetos do mundo)
            │   ├── Player
            │   ├── Camera
            │   ├── Chicken (Anselma)
            │   ├── Lista de Platform
            │   ├── Lista de SolidPlatform
            │   ├── Lista de Door
            │   ├── Lista de Gate
            │   ├── Lista de Screw
            │   ├── Lista de ToolTip
            │   ├── Lista de Decoration
            │   ├── Lista de Crate
            │   ├── Lista de Enemy
            │   └── Punktauro* (boss - ponteiro único)
            │
            ├── HUD (interface do usuário)
            │
            ├── GameState (enum: MENU, PLAYING, GAME_OVER, PAUSED, CREDITS, SCORE_ENTRY, HIGH_SCORES)
            │
            ├── StartMenu
            ├── PauseMenu
            ├── GameOverScreen
            ├── CreditsScreen
            ├── ScoreEntryScreen
            ├── HighScoresScreen
            └── HighScoresManager (persistência de pontuações)
```

### 4.4 Sistemas Principais

#### Loop Principal (main.cpp)

```
while (gameManager.isRunning()) {
    1. Calcular deltaTime
    2. Processar eventos (handleEvents)
    3. Atualizar estado (update)
    4. Limpar tela (SDL_RenderClear)
    5. Renderizar (render)
    6. Aplicar efeito CRT (scanlines + vinheta)
    7. Apresentar na tela (SDL_RenderPresent)
}
```

#### Características de Renderização

| Propriedade | Valor |
|-------------|-------|
| Resolução | 1280x720 pixels |
| VSync | Habilitado |
| Taxa de quadros | 60 FPS (sincronizado) |
| Efeito CRT | Scanlines + Vinheta |

---

## 5. Componentes Principais

### 5.1 GameManager

**Arquivo:** `include/arscrew/GameManager.h`

**Responsabilidades:**
- Inicialização do SDL e subsistemas
- Gerenciamento do loop principal
- Controle de transições entre estados do jogo
- Gerenciamento de áudio (música e efeitos sonoros)
- Renderização de backgrounds com parallax
- Atualização do HUD
- Cálculo do sistema de pontuação

**Atributos Principais:**

```cpp
GameWorld mWorld;              // Mundo do jogo
GameState mCurrentState;       // Estado atual
SDL_Renderer* mRenderer;       // Renderizador SDL

// Sistema de áudio
Mix_Music* mBackgroundMusic;
Mix_Chunk* mJumpSound;
Mix_Chunk* mAttackSound;
Mix_Chunk* mHurtSound;
// ... outros sons

// Sistema de parallax (5 camadas)
SDL_Texture* mParallaxLayers[5];

// Estatísticas para pontuação
int mEnemiesKilled;
int mDamageReceived;
float mBestAirTime;
float mTotalGameTime;
int mGameOvers;
```

### 5.2 GameWorld

**Arquivo:** `include/arscrew/GameWorld.h`

**Responsabilidades:**
- Carregar níveis do formato TMX (Tiled)
- Gerenciar todas as entidades do mundo
- Atualizar física e animações
- Renderizar objetos
- Processar colisões via CollisionEngine
- Gerenciar entrada do usuário

**Sistema de Callbacks de Som:**

O GameWorld utiliza callbacks para comunicar eventos de som ao GameManager:

```cpp
std::function<void()> onAttack;
std::function<void()> onToolTipCollected;
std::function<void()> onPlayerHurt;
std::function<void()> onEnemyHurt;
std::function<void()> onEnemyDead;
std::function<void()> onBossAccelerate;
std::function<void()> onBossJump;
std::function<void()> onBossDead;
std::function<void()> onGateOpen;
```

### 5.3 Player

**Arquivo:** `include/arscrew/Player.h`

**Características:**

| Propriedade | Valor |
|-------------|-------|
| HP Máximo | 100 |
| Velocidade de Movimento | 200 u/s |
| Força de Pulo | -250 u/s² |
| Gravidade | 580 u/s² |
| Fricção | 0.85 |

**Sistema de Ataque:**

```cpp
enum class AttackType {
    CUTTING,    // Ataque cortante (para parafusos FLATHEAD)
    PIERCING    // Ataque perfurante (para parafusos PHILLIPS)
};
```

**Mecânicas:**
- Movimentação horizontal (esquerda/direita)
- Pulo simples
- Wall-jump (pulo na parede)
- Dois tipos de ataque (alternar com Tab ou botão X do gamepad)
- Invulnerabilidade temporária após receber dano
- Animações: parado, correndo, pulando, atacando, caindo

**Hitboxes:**
- `mAttackHitbox`: Define área de ataque
- `mHurtbox`: Define área onde pode receber dano

### 5.4 Enemy

**Arquivo:** `include/arscrew/Enemy.h`

**Estados de IA:**

```cpp
enum class EnemyState {
    IDLE,       // Parado/patrulhando
    FOLLOWING,  // Seguindo o jogador
    ATTACKING,  // Atacando
    STUNNED     // Atordoado
};
```

**Parâmetros:**

| Propriedade | Valor |
|-------------|-------|
| HP | 100 |
| Invulnerabilidade | 1.0s após dano |
| Duração de Ataque | 0.5s |
| Cooldown de Ataque | 1.5s |
| Duração de Stun | 0.5s |

### 5.5 Punktauro (Boss)

**Arquivo:** `include/arscrew/Punktauro.h`

**Herda de:** Enemy

**Fases de Combate:**

```cpp
enum class PunktauroState {
    PHASE_1,    // Fase normal - seguir e atacar
    PHASE_2,    // Fase intermediária - mais agressivo (ativa em 30% HP)
    PHASE_3,    // Fase final - padrões de ataque especiais
    DEFEATED    // Derrotado
};
```

**Características Especiais:**
- Transições de fase baseadas em percentual de vida
- Ataque especial com cooldown
- Padrões de movimento customizados por fase
- Mecânica de "charging" (carregamento)
- Hurtbox especial da cabeça (permite acertos críticos)
- Escala visual dinâmica
- Sons únicos (aceleração, pulo, morte)

### 5.6 Chicken (Anselma)

**Arquivo:** `include/arscrew/Chicken.h`

**Parâmetros Padrão:**

| Propriedade | Valor |
|-------------|-------|
| Velocidade | 120 u/s |
| Distância de Seguimento | 50 unidades |
| Distância de Parada | 30 unidades |

**Funcionalidades:**
- Segue o jogador automaticamente
- Animações de movimento
- Não interage com colisões (atravessa objetos)
- Elemento visual/narrativo de companionship

### 5.7 Screw (Parafusos)

**Arquivo:** `include/arscrew/Screw.h`

**Tipos:**

```cpp
enum class ScrewType {
    FLATHEAD,   // Cabeça chata → destruído com ataque CUTTING
    PHILLIPS    // Cabeça Phillips → destruído com ataque PIERCING
};
```

**Sistema de Respawn:**
- Pode ser destruído e reaparecer após tempo configurável
- Respawn habilitável/desabilitável por instância
- Timer de respawn configurável

### 5.8 Gate (Portões)

**Arquivo:** `include/arscrew/Gate.h`

**Estados:**

```cpp
enum class GateState {
    CLOSED,     // Fechado - bloqueia passagem
    OPENING,    // Abrindo - animação em andamento
    OPEN,       // Aberto - permite passagem
    CLOSING     // Fechando - animação em andamento
};
```

**Mecânica:**
- Cada portão é vinculado a um parafuso específico (`mTargetScrew`)
- Quando o parafuso alvo é destruído, o portão inicia animação de abertura
- Velocidade de abertura customizável
- Sprites separados para estado aberto e fechado
- Callback de som ao abrir

### 5.9 ToolTip (Pontas de Ferramentas)

**Tipos:** FLATHEAD ou PHILLIPS

**Mecânica:**
- Objetos coletáveis espalhados pelos níveis
- Ao coletar, o jogador ganha a habilidade de usar aquele tipo de ataque
- Necessário coletar ambos os tipos para máxima flexibilidade

### 5.10 HUD

**Arquivo:** `include/arscrew/HUD.h`

**Elementos Renderizados:**
1. Tipo de ataque atual (CUTTING ou PIERCING)
2. Instruções de controles
3. Barra de vida do jogador
4. Barra de vida do boss (quando presente)
5. Efeitos visuais retrô (scanlines)

**Paleta de Cores Retrô:**
- Orange, Blue, Green, Red, Cyan, Yellow, Purple

### 5.11 Menus e Telas

#### StartMenu
- Opções: START GAME, HIGH SCORES, CREDITS, QUIT
- Efeito de fade in na entrada
- Animação de blink no item selecionado

#### PauseMenu
- Ativado durante gameplay
- Opções para retomar ou sair

#### GameOverScreen
- Exibido quando o jogador morre
- Mostra pontuação final
- Opções para reiniciar ou voltar ao menu

#### CreditsScreen
- Exibido após derrotar o boss
- Efeito de scroll vertical dos créditos
- Fade de saída

#### HighScoresScreen
- Exibe top 10 pontuações
- Navegação para voltar ao menu

#### ScoreEntryScreen
- Entrada de iniciais do jogador (3 caracteres)
- Ativado quando pontuação entra no ranking

### 5.12 HighScoresManager

**Arquivo:** `include/arscrew/HighScoresManager.h`

**Funcionalidades:**
- Gerencia lista de top 10 pontuações
- Persistência em arquivo `highscores.txt`
- Verifica se nova pontuação entra no ranking
- Adiciona novas entradas ordenadamente

**Métricas Rastreadas para Pontuação:**
- Inimigos eliminados
- Dano total recebido
- Melhor tempo no ar
- Tempo total de jogo
- Contagem de game overs

---

## 6. Sistema de Build

### 6.1 Makefile

**Localização:** `/home/netorapg/projects/A-Rusty-Screw/Makefile`

**Configuração Principal:**

```makefile
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/local/include/SDL2
LDFLAGS = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -ltinyxml2
```

**Targets Disponíveis:**

| Target | Descrição |
|--------|-----------|
| `all` | Compila o projeto (padrão) |
| `clean` | Remove arquivos compilados |
| `mingw` | Compila para Windows (cross-compile) |

**Arquivos Gerados:**
- Executável: `bin/main`
- Objetos: `obj/*.o`

### 6.2 Script de Execução

**run.sh:**
```bash
#!/bin/bash
./bin/main
```

### 6.3 Comandos de Build

```bash
# Compilar o projeto
make

# Limpar arquivos compilados
make clean

# Executar o jogo
./run.sh
# ou
./bin/main
```

---

## 7. Dependências Externas

### 7.1 Dependências Obrigatórias

| Biblioteca | Propósito | Instalação (Arch/Manjaro) |
|------------|-----------|---------------------------|
| SDL2 | Rendering, eventos, input | `pacman -S sdl2` |
| SDL2_image | Carregamento de imagens | `pacman -S sdl2_image` |
| SDL2_ttf | Renderização de fontes | `pacman -S sdl2_ttf` |
| SDL2_mixer | Sistema de áudio | `pacman -S sdl2_mixer` |
| tinyxml2 | Parsing de XML (TMX) | `pacman -S tinyxml2` |
| g++ | Compilador C++20 | `pacman -S gcc` |

### 7.2 Biblioteca Bundled

| Biblioteca | Descrição |
|------------|-----------|
| MyLib | Biblioteca customizada incluída no projeto (Vector2D, utilitários) |

### 7.3 Arquivos Ignorados (.gitignore)

```
bin/
obj/
SDL/
```

---

## 8. Sistema de Assets

### 8.1 Sprites de Personagens

| Arquivo | Descrição |
|---------|-----------|
| `bezourinha_parada.png` | Sprite parado (32x32) |
| `bezourinha_correndo.png` | Animação correndo |
| `bezourinha_pulando.png` | Sprite pulando |
| `bezourinha_animacao.png` | Animação genérica |
| `bezourinha_sprites.ase/.png` | Spritesheet original |
| `enemy_sprite.png` | Sprites dos inimigos |
| `Anselma.png` | Sprite da galinha |

### 8.2 Sprites do Boss (Punktauro/Keren)

| Arquivo | Descrição |
|---------|-----------|
| `KEREN.png` | Sprite principal |
| `Keren_left.png` | Virado para esquerda |
| `Keren_right.png` | Virado para direita |
| `Keren_up.png` | Olhando para cima |
| `Keren_fall.png` | Caindo |
| `Keren_dead.png` | Morto |

### 8.3 Tileset

| Arquivo | Descrição |
|---------|-----------|
| `fulltile.png` | Tileset principal (490x503px, 225 tiles de 32x32) |
| `fulltile.tsx` | Definição XML do tileset |

### 8.4 Backgrounds e Efeitos

| Arquivo | Descrição |
|---------|-----------|
| `Background.png` | Background principal |
| `factory.png` | Sprite de fábrica |
| `parallax/1-5.png` | 5 camadas de parallax |

### 8.5 Áudio - Música

| Arquivo | Descrição |
|---------|-----------|
| `8-bit-game-158815.mp3` | Música tema (estilo 8-bit) |
| `backgroundsong.mp3` | Música de fundo alternativa |

### 8.6 Áudio - Efeitos Sonoros

| Arquivo | Descrição |
|---------|-----------|
| `jump-sound.wav` | Som de pulo |
| `accelerate.wav` | Som de aceleração |
| `clank.wav` | Som de ataque/colisão metálica |
| `enemy-hitted.wav` | Som de inimigo levando dano |
| `enemy-dead.wav` | Som de inimigo morrendo |
| `boss-jump.wav` | Som do boss pulando |
| `boss-explode.wav` | Som do boss morrendo |
| `found-upgrade.wav` | Som de coleta de item |
| `gate-sound.wav` | Som de portão abrindo |
| `hurt-sound.wav` | Som de jogador levando dano |

### 8.7 Fontes

| Arquivo | Estilo |
|---------|--------|
| `Arial.ttf` | Fonte padrão |
| `All Star Resort.ttf` | Fonte decorativa |
| `Megadeth.ttf` | Fonte para títulos |
| `METALORD.TTF` | Fonte metálica |
| `Type Machine.ttf` | Fonte de máquina de escrever |

---

## 9. Sistema de Mapas (Níveis)

### 9.1 Formato

O projeto utiliza o formato **TMX** (Tiled Map XML) do editor **Tiled Map Editor**.

### 9.2 Lista de Níveis

| Arquivo | Tipo | Descrição |
|---------|------|-----------|
| `level1.tmx` | Campanha | Nível 1 - Introdução |
| `level2.tmx` | Campanha | Nível 2 |
| `level3.tmx` | Campanha | Nível 3 |
| `level4.tmx` | Campanha | Nível 4 |
| `level5.tmx` | Campanha | Nível 5 |
| `level6.tmx` | Campanha | Nível 6 |
| `level7.tmx` | Campanha | Nível 7 - Último antes do boss |
| `bossarena.tmx` | Especial | Arena do boss final |
| `demomap.tmx` | Demo | Mapa de demonstração |
| `demoroom1-4.tmx` | Demo | Salas de demonstração |
| `demoroom6.tmx` | Demo | Sala de demonstração 6 |
| `gate_example.tmx` | Exemplo | Demonstração de portões |
| `hole.tmx` | Especial | Mapa com buracos |
| `mapabanca1-3.tmx` | Alternativo | Mapas alternativos |
| `tooltip_example.tmx` | Exemplo | Demonstração de coleta |

### 9.3 Estrutura de um Nível TMX

**Exemplo: level1.tmx**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<map version="1.10" orientation="orthogonal"
     width="40" height="25"
     tilewidth="32" tileheight="32">

    <!-- Tileset -->
    <tileset firstgid="1" source="fulltile.tsx"/>

    <!-- Camada de colisão -->
    <layer name="blocks" width="40" height="25">
        <data encoding="csv">
            <!-- IDs dos tiles -->
        </data>
    </layer>

    <!-- Camada de decoração -->
    <layer name="decorations" width="40" height="25">
        <data encoding="csv">
            <!-- IDs dos tiles -->
        </data>
    </layer>

    <!-- Grupo de objetos: Spawns -->
    <objectgroup name="Spawns">
        <object type="player_spawn" x="64" y="576"/>
        <object type="enemy_spawn" x="640" y="576"/>
    </objectgroup>

    <!-- Grupo de objetos: Objects -->
    <objectgroup name="Objects">
        <object type="door" x="1280" y="576">
            <properties>
                <property name="target" value="../map/level2.tmx"/>
                <property name="spawn_x" type="float" value="3"/>
                <property name="spawn_y" type="float" value="3"/>
            </properties>
        </object>
        <object type="screw_flathead" x="512" y="480"/>
        <object type="gate" x="800" y="480">
            <properties>
                <property name="target_screw" type="int" value="0"/>
            </properties>
        </object>
    </objectgroup>
</map>
```

### 9.4 Tipos de Objetos

| Tipo | Descrição |
|------|-----------|
| `player_spawn` | Ponto de spawn do jogador |
| `enemy_spawn` | Ponto de spawn de inimigos |
| `door` | Porta de transição de nível |
| `screw_flathead` | Parafuso tipo flathead |
| `screw_phillips` | Parafuso tipo phillips |
| `gate` | Portão vinculado a parafuso |
| `tooltip_flathead` | Ponta flathead coletável |
| `tooltip_phillips` | Ponta phillips coletável |
| `crate` | Caixote empurrável |
| `decoration` | Decoração visual |

### 9.5 Propriedades de Objetos

**Door (Porta):**
- `target`: Caminho do próximo nível
- `spawn_x`: Posição X do spawn no próximo nível
- `spawn_y`: Posição Y do spawn no próximo nível

**Gate (Portão):**
- `target_screw`: Índice do parafuso que controla este portão

---

## 10. Fluxo do Jogo

### 10.1 Diagrama de Estados

```
                    ┌──────────────────────────────────────┐
                    │                                      │
                    ▼                                      │
┌─────────┐    ┌─────────┐    ┌─────────┐    ┌──────────┐ │
│  MENU   │───▶│ PLAYING │───▶│ PAUSED  │───▶│   MENU   │ │
└─────────┘    └─────────┘    └─────────┘    └──────────┘ │
    │              │              │                        │
    │              │              └─────────┐              │
    │              ▼                        │              │
    │         ┌─────────┐              (retomar)           │
    │         │GAME_OVER│                   │              │
    │         └─────────┘                   │              │
    │              │                        │              │
    │              ▼                        │              │
    │       ┌────────────┐                  │              │
    │       │SCORE_ENTRY │                  │              │
    │       └────────────┘                  │              │
    │              │                        │              │
    │              ▼                        │              │
    │       ┌────────────┐                  │              │
    ├──────▶│HIGH_SCORES │◀─────────────────┘              │
    │       └────────────┘                                 │
    │              │                                       │
    │              └───────────────────────────────────────┘
    │
    │         ┌─────────┐
    └────────▶│ CREDITS │──────────────────────────────────┐
              └─────────┘                                  │
                                                           │
              ┌─────────┐                                  │
              │  QUIT   │◀─────────────────────────────────┘
              └─────────┘
```

### 10.2 Progressão de Gameplay

```
1. Menu Principal
       │
       ▼
2. level1.tmx (Introdução)
       │
       ▼
3. level2.tmx - level7.tmx (Progressão)
       │
       ▼
4. bossarena.tmx (Boss Fight - Punktauro)
       │
       ▼
5. Créditos
       │
       ▼
6. High Scores (se pontuação qualifica)
       │
       ▼
7. Menu Principal
```

### 10.3 Loop de Gameplay Típico

1. Jogador inicia no spawn point
2. Explora o nível, encontra parafusos e inimigos
3. Coleta ToolTips para desbloquear tipos de ataque
4. Destrói parafusos com o tipo correto de ataque
5. Portões vinculados aos parafusos abrem
6. Alcança a porta de saída (Door)
7. Transição com fade para próximo nível
8. Repetir até boss arena
9. Derrota Punktauro
10. Créditos rolam
11. Entrada de high score (se aplicável)

---

## 11. Mecânicas do Jogo

### 11.1 Movimentação do Jogador

**Controles de Teclado:**

| Tecla | Ação |
|-------|------|
| A / Seta Esquerda | Mover para esquerda |
| D / Seta Direita | Mover para direita |
| Space / W / Seta Cima | Pular |
| Tab | Alternar tipo de ataque |
| Enter / Z | Atacar |
| Escape | Pausar |

**Controles de Gamepad:**

| Botão | Ação |
|-------|------|
| Analog Stick / D-Pad | Movimentação |
| A | Pular |
| X | Alternar tipo de ataque |
| B | Atacar |
| Start | Pausar |

**Parâmetros de Física:**

| Propriedade | Valor | Descrição |
|-------------|-------|-----------|
| Velocidade Horizontal | 200 u/s | Velocidade de corrida |
| Força de Pulo | -250 u/s² | Impulso vertical (negativo = para cima) |
| Gravidade | 580 u/s² | Aceleração para baixo |
| Fricção | 0.85 | Desaceleração horizontal |

### 11.2 Sistema de Ataque Dual-Tool

**Conceito:**
O jogo possui dois tipos de ataques, cada um efetivo contra um tipo específico de parafuso.

```
┌─────────────────┐         ┌─────────────────┐
│  Ataque CUTTING │         │ Ataque PIERCING │
│    (Cortante)   │         │  (Perfurante)   │
└────────┬────────┘         └────────┬────────┘
         │                           │
         ▼                           ▼
┌─────────────────┐         ┌─────────────────┐
│ Parafuso        │         │ Parafuso        │
│ FLATHEAD        │         │ PHILLIPS        │
│ (Cabeça Chata)  │         │ (Cabeça Cruz)   │
└─────────────────┘         └─────────────────┘
```

**Fluxo de Coleta:**
1. Inicialmente, jogador não tem nenhum tipo de ataque
2. Encontra ToolTip FLATHEAD → desbloqueia ataque CUTTING
3. Encontra ToolTip PHILLIPS → desbloqueia ataque PIERCING
4. Pode alternar entre tipos com Tab/X
5. HUD mostra tipo selecionado atualmente

### 11.3 Sistema de Colisões

**Tipos de Plataformas:**

| Tipo | Comportamento |
|------|---------------|
| Platform | Pode passar por baixo pulando; para em cima |
| SolidPlatform | Bloqueia em todas as direções |
| Crate | Pode ser empurrado horizontalmente |

**Mecânicas de Colisão:**
- Detecção AABB (Axis-Aligned Bounding Box)
- Resolução de penetração
- Colisão lateral (parede) para wall-jump
- Detecção de chão para permitir pulo
- Pass-through em plataformas normais (de baixo para cima)

### 11.4 Comportamento de Inimigos

**Máquina de Estados:**

```
         ┌──────────────────────────────────────┐
         │                                      │
         ▼                                      │
     ┌──────┐    jogador próximo    ┌─────────┐ │
     │ IDLE │──────────────────────▶│FOLLOWING│ │
     └──────┘                       └─────────┘ │
         ▲                              │       │
         │                              │       │
         │            em range de ataque│       │
         │                              ▼       │
         │                        ┌──────────┐  │
         │                        │ATTACKING │  │
         │                        └──────────┘  │
         │                              │       │
         │                    recebe dano│      │
         │                              ▼       │
         │                        ┌─────────┐   │
         └────────────────────────│ STUNNED │───┘
               após 0.5s          └─────────┘
```

### 11.5 Boss Punktauro

**Sistema de Fases:**

| Fase | HP Restante | Comportamento |
|------|-------------|---------------|
| PHASE_1 | 100% - 60% | Normal - seguir e atacar |
| PHASE_2 | 60% - 30% | Mais agressivo, ataques mais rápidos |
| PHASE_3 | 30% - 0% | Padrões especiais, charging |
| DEFEATED | 0% | Animação de morte, transição para créditos |

**Mecânicas Especiais:**
- Hurtbox da cabeça para acertos críticos
- Ataque de charging (carrega em direção ao jogador)
- Invulnerabilidade durante transições de fase
- Sons únicos de voz

### 11.6 Sistema de Portões

**Mecânica de Vinculação:**

```
┌─────────────┐    destruir    ┌─────────────┐
│   Parafuso  │───────────────▶│   Portão    │
│  (target)   │                │   OPENING   │
└─────────────┘                └─────────────┘
                                      │
                                      ▼
                               ┌─────────────┐
                               │   Portão    │
                               │    OPEN     │
                               └─────────────┘
```

---

## 12. Funcionalidades Especiais

### 12.1 Efeito CRT (Cathode Ray Tube)

**Componentes:**

1. **Scanlines**
   - Linhas horizontais semitransparentes
   - Alpha: 15 (quase imperceptível)
   - Simula linhas de varredura de monitores CRT

2. **Vinheta**
   - Escurecimento gradual nas bordas
   - Alpha: 30
   - Topo/base e laterais com gradiente
   - Cria efeito de curvatura de tela antiga

**Aplicação:** Renderizado após todos os outros elementos, antes de SDL_RenderPresent

### 12.2 Sistema Parallax

**Estrutura de 5 Camadas:**

| Camada | Velocidade | Descrição |
|--------|------------|-----------|
| 1 | Mais rápida | Elementos mais próximos |
| 2 | Rápida | Segundo plano próximo |
| 3 | Média | Plano médio |
| 4 | Lenta | Segundo plano distante |
| 5 | Mais lenta | Elementos mais distantes |

**Fórmula:**
```
offset_camada = posicao_camera * fator_parallax[camada]
```

### 12.3 Câmera

**Características:**
- Follow suave com interpolação linear (lerp)
- Margens para melhor visibilidade do caminho
- Efeito de shake (tremor) para feedback de impacto
- Limites para não mostrar fora do mapa

### 12.4 Sistema de Pontuação

**Métricas Coletadas:**

| Métrica | Efeito na Pontuação |
|---------|---------------------|
| Inimigos Eliminados | Positivo (+) |
| Dano Evitado | Positivo (+) |
| Melhor Tempo no Ar | Positivo (+) |
| Tempo Total de Jogo | Influencia cálculo |
| Game Overs | Penalidade (-) |

**Persistência:**
- Top 10 pontuações salvas em `highscores.txt`
- Formato: nome (3 caracteres) + pontuação
- Ordenação decrescente por pontuação

---

## 13. Padrões de Design

### 13.1 Padrões Identificados

| Padrão | Uso no Projeto |
|--------|----------------|
| **Object-Oriented Design** | Hierarquia Object → StaticObject/DynamicObject |
| **Manager Pattern** | GameManager, HighScoresManager |
| **Component Pattern** | Entidades com comportamentos modulares |
| **State Pattern** | GameState, EnemyState, GateState, PunktauroState |
| **Factory Pattern** | GameWorld cria entidades ao carregar TMX |
| **Callback Pattern** | GameWorld → GameManager para eventos de som |
| **Observer Pattern** | HUD observa Player e Punktauro |

### 13.2 Exemplos de Implementação

**State Pattern (GameState):**
```cpp
enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    PAUSED,
    CREDITS,
    SCORE_ENTRY,
    HIGH_SCORES
};

// Uso no GameManager
switch (mCurrentState) {
    case GameState::MENU:
        mStartMenu.update();
        break;
    case GameState::PLAYING:
        mWorld.update(deltaTime);
        break;
    // ...
}
```

**Callback Pattern:**
```cpp
// GameWorld.h
std::function<void()> onEnemyDead;

// GameWorld.cpp
void GameWorld::onEnemyKilled() {
    if (onEnemyDead) onEnemyDead();
}

// GameManager.cpp
mWorld.onEnemyDead = [this]() {
    Mix_PlayChannel(-1, mEnemyDeadSound, 0);
    mEnemiesKilled++;
};
```

---

## 14. Código-Chave

### 14.1 Inicialização SDL (main.cpp)

```cpp
// Inicialização dos subsistemas
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
TTF_Init();
Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

// Criação da janela
SDL_Window* window = SDL_CreateWindow(
    "A Rusty Screw",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1280, 720,
    SDL_WINDOW_SHOWN
);

// Criação do renderizador com VSync
SDL_Renderer* renderer = SDL_CreateRenderer(
    window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
);
```

### 14.2 Loop Principal (main.cpp)

```cpp
Uint32 lastTime = SDL_GetTicks();

while (gameManager.isRunning()) {
    // Calcular deltaTime
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    // Processar eventos
    gameManager.handleEvents();

    // Atualizar estado
    gameManager.update(deltaTime);

    // Limpar tela
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Renderizar
    gameManager.render();

    // Aplicar efeito CRT
    applyCRTEffect(renderer);

    // Apresentar na tela
    SDL_RenderPresent(renderer);
}
```

### 14.3 Carregamento de Nível (GameWorld)

```cpp
void GameWorld::loadLevelFromTMX(const std::string& filePath) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filePath.c_str());

    XMLElement* map = doc.FirstChildElement("map");

    // Processar camadas de tiles
    for (XMLElement* layer = map->FirstChildElement("layer");
         layer != nullptr;
         layer = layer->NextSiblingElement("layer")) {
        processLayer(layer);
    }

    // Processar grupos de objetos
    for (XMLElement* objGroup = map->FirstChildElement("objectgroup");
         objGroup != nullptr;
         objGroup = objGroup->NextSiblingElement("objectgroup")) {
        processObjectGroup(objGroup);
    }
}
```

### 14.4 Efeito CRT

```cpp
void applyCRTEffect(SDL_Renderer* renderer) {
    // Scanlines
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 15);
    for (int y = 0; y < 720; y += 2) {
        SDL_RenderDrawLine(renderer, 0, y, 1280, y);
    }

    // Vinheta
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 30);
    // Renderizar gradientes nas bordas
    // ...
}
```

---

## 15. Estado Atual do Projeto

### 15.1 Informações do Repositório

| Propriedade | Valor |
|-------------|-------|
| Branch Atual | `dev` |
| Branch Principal | `main` |

### 15.2 Arquivos Modificados (Não Commitados)

```
M include/arscrew/HighScoresManager.h
M include/arscrew/HighScoresScreen.h
M include/arscrew/ScoreEntryScreen.h
M src/GameManager.cpp
```

### 15.3 Histórico de Commits Recentes

| Hash | Mensagem |
|------|----------|
| `f69214c` | minor changes |
| `158f00a` | trabalhando no sistema de pontuação |
| `827a584` | correções na movimentação e também trabalhando no level design do modo challenge |
| `edf38f2` | Trabalhando no sistema de save do score |
| `b483e2b` | minor changes |

### 15.4 Foco de Desenvolvimento Atual

Baseado nos commits recentes e arquivos modificados:
- Sistema de pontuação e high scores
- Tela de entrada de pontuação
- Persistência de pontuações
- Ajustes no level design

---

## 16. Conclusões

### 16.1 Pontos Fortes

1. **Arquitetura Sólida**
   - Hierarquia de classes bem definida
   - Separação clara de responsabilidades
   - Uso de padrões de design apropriados

2. **Código Moderno**
   - C++20 com práticas atuais
   - Smart pointers para gerenciamento de memória
   - Lambdas e callbacks para desacoplamento

3. **Extensibilidade**
   - Fácil adicionar novos tipos de objetos
   - Sistema de níveis baseado em dados (TMX)
   - Modularidade permite expansão

4. **Completude**
   - Sistema de áudio completo (música + SFX)
   - Múltiplos estados de jogo
   - Persistência de dados (high scores)
   - Suporte a múltiplos dispositivos de entrada

5. **Estética Coerente**
   - Efeitos visuais retrô (CRT, scanlines)
   - Paleta de cores consistente
   - Fontes temáticas

### 16.2 Potencial de Expansão

| Funcionalidade | Descrição |
|----------------|-----------|
| Novos Inimigos | Adicionar tipos com comportamentos diferentes |
| Power-ups | Sistema de melhorias temporárias |
| Cutscenes | Cinemáticas entre níveis |
| Modo Challenge | Níveis especiais com objetivos |
| Múltiplos Personagens | Personagens jogáveis alternativos |
| Achievements | Sistema de conquistas |
| Localização | Suporte a múltiplos idiomas |

### 16.3 Avaliação Técnica

O projeto "A Rusty Screw" representa um **jogo de plataforma indie completo e funcional**, demonstrando:

- Compreensão sólida de desenvolvimento de jogos
- Domínio da linguagem C++ e biblioteca SDL2
- Capacidade de implementar sistemas complexos (física, colisão, IA)
- Atenção a detalhes de game feel (câmera, efeitos, feedback)
- Organização e documentação adequadas

O código está em um estado maduro, pronto para polimento final e release.

---

*Documento gerado automaticamente por análise de código.*
