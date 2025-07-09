# 🔧 Sistema de Pontas (ToolTips) - Guia Completo

## 🎯 Visão Geral

O sistema de pontas adiciona uma mecânica de progressão ao jogo onde o jogador deve coletar diferentes tipos de pontas para poder usar diferentes ataques contra parafusos específicos.

## 🛠️ Tipos de Pontas

### 🟡 Ponta Flathead (Fenda)
- **Cor**: Dourado/Amarelo
- **Habilita**: Ataques cortantes (CUTTING)
- **Usado contra**: Parafusos de fenda (`screw_flathead`)
- **Status inicial**: Player já inicia com esta ponta

### 🔵 Ponta Phillips (Cruz)
- **Cor**: Azul metálico
- **Habilita**: Ataques perfurantes (PIERCING)
- **Usado contra**: Parafusos Phillips (`screw_phillips`)
- **Status inicial**: Deve ser coletada no cenário

## 🎮 Mecânica de Jogo

### Sistema de Restrição
1. **Sem ponta Phillips**: Player não pode usar ataques perfurantes
2. **Tentativa de ataque inválido**: Mensagem de erro e ataque não executa
3. **Coleta de ponta**: Automaticamente habilita o tipo de ataque correspondente

### Progressão Sugerida
```
Início → Flathead (padrão) → Coletar Phillips → Ambos os ataques disponíveis
```

## 📍 Como Spawnar no Editor Tiled

### 1. Ponta Phillips (Cruz)
```xml
<object id="2" type="tooltip_phillips" x="320" y="350"/>
```

### 2. Ponta Flathead (Fenda) - Opcional
```xml
<object id="3" type="tooltip_flathead" x="150" y="300"/>
```

### 3. Exemplo Completo de Puzzle
```xml
<!-- Player spawn -->
<object id="1" type="player_spawn" x="96" y="384"/>

<!-- Ponta Phillips para coletar -->
<object id="2" type="tooltip_phillips" x="320" y="350"/>

<!-- Parafuso Phillips que só pode ser destruído após coletar a ponta -->
<object id="3" type="screw_phillips" x="480" y="200"/>

<!-- Portão que abre quando o parafuso Phillips for destruído -->
<object id="4" type="gate" x="288" y="320" width="32" height="96">
 <properties>
  <property name="target_screw_x" type="float" value="480"/>
  <property name="target_screw_y" type="float" value="200"/>
 </properties>
</object>
```

## 🎨 Aparência Visual

### Características das Pontas
- **Tamanho**: 24x24 pixels
- **Efeito**: Flutuação suave (8 pixels de amplitude)
- **Brilho**: Efeito de brilho sutil no canto superior
- **Animação**: Movimento senoidal contínuo

### Cores e Padrões
- **Flathead**: Dourado com linha horizontal escura
- **Phillips**: Azul aço com cruz azul escura

## 🔄 Estados e Comportamentos

### Estados da Ponta
1. **Disponível**: Visível e coletável
2. **Coletada**: Invisível e sem colisão

### Comportamentos do Player
1. **Início**: Pode usar apenas ataques cortantes
2. **Após coletar Phillips**: Pode alternar entre os dois tipos
3. **Tentativa de ataque sem ponta**: Bloqueado com mensagem

## 📝 Mensagens do Sistema

### Coleta Bem-sucedida
```
"Collected PHILLIPS tool tip! Can now use PIERCING attacks."
"Automatically switched to PIERCING attack type!"
```

### Tentativa de Ataque Inválido
```
"Cannot attack! Missing PHILLIPS tool tip!"
"Cannot switch to PIERCING attack - Phillips tool tip not collected!"
```

### Debug/Status
```
"Player initialized with FLATHEAD tool tip (CUTTING attacks enabled)"
"Attacking with CUTTING attack!"
"Attacking with PIERCING attack!"
```

## 🎯 Design de Níveis

### Nível Introdutório
1. Player inicia com Flathead
2. Alguns parafusos Flathead fáceis
3. Ponta Phillips visível mas em local elevado
4. Parafuso Phillips bloqueia progressão

### Nível Intermediário
1. Múltiplas pontas espalhadas
2. Puzzles que requerem alternância entre tipos
3. Portões controlados por diferentes tipos de parafusos

### Nível Avançado
1. Pontas em locais difíceis de alcançar
2. Sequências complexas de parafusos
3. Timing crítico para alternar entre ataques

## 🔧 Implementação Técnica

### Arquivos Modificados
- `include/arscrew/ToolTip.h` - Definição da classe
- `src/ToolTip.cpp` - Implementação da classe
- `include/arscrew/Player.h` - Sistema de coleta
- `src/Player.cpp` - Lógica de restrição de ataques
- `include/arscrew/GameWorld.h` - Integração
- `src/GameWorld.cpp` - Processamento e colisões

### Sistema de Colisão
- Detecção automática Player ↔ ToolTip
- Coleta instantânea ao tocar
- Remoção visual imediata

### Persistência
- Pontas coletadas mantidas durante o nível
- Reset ao recarregar nível
- Compatível com sistema de save/load futuro

## 🎮 Exemplo de Jogo

### Cenário: "A Ponta Perdida"
1. **Objetivo**: Abrir portão para área secreta
2. **Obstáculo**: Portão controlado por parafuso Phillips
3. **Solução**: Encontrar e coletar ponta Phillips
4. **Recompensa**: Acesso a área com power-ups

### Fluxo de Gameplay
```
Spawn → Explorar → Encontrar ponta Phillips → Coletar → 
Alternar para ataque perfurante → Destruir parafuso Phillips → 
Portão abre → Progresso!
```

## 🚀 Compilação e Teste

### Build
```bash
cd /home/netorapg/projects/A-Rusty-Screw
make clean && make
```

### Teste Rápido
1. Carregue `map/tooltip_example.tmx`
2. Tente atacar parafuso Phillips (deve falhar)
3. Colete ponta Phillips azul
4. Troque para ataque perfurante (Ctrl)
5. Ataque parafuso Phillips (deve funcionar)
6. Portão deve abrir!

**🎉 Sistema completo e funcional!**
