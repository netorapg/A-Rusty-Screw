# Como Spawnar Portões no Editor Tiled

## Passo a Passo para Criar um Portão Controlado por Parafuso

### 1. Abrir o Editor Tiled
- Abra o Tiled Map Editor
- Carregue seu mapa TMX ou crie um novo

### 2. Criar uma Camada de Objetos
Se ainda não tiver, crie uma camada de objetos:
- Clique em **Layer** → **Add Layer** → **Object Layer**
- Nomeie como "spawns" ou "objects"

### 3. Adicionar o Parafuso Controlador

1. **Selecione a ferramenta de objetos** (ícone de retângulo)
2. **Desenhe um retângulo** onde você quer o parafuso
3. **Configure as propriedades** do objeto:
   - **Name**: `control_screw` (opcional)
   - **Type**: `screw_flathead` ou `screw_phillips`
   - **Posição**: Anote as coordenadas X e Y

### 4. Adicionar o Portão

1. **Desenhe outro retângulo** onde você quer o portão
2. **Configure as propriedades** do objeto:
   - **Name**: `gate1` (opcional)
   - **Type**: `gate`
   - **Width**: Largura do portão (ex: 32 pixels)
   - **Height**: Altura do portão (ex: 96 pixels)

3. **Adicionar Propriedades Customizadas**:
   - Clique no botão **+** na aba Properties
   - Adicione as seguintes propriedades:

| Nome | Tipo | Valor |
|------|------|-------|
| `target_screw_x` | float | Coordenada X do parafuso |
| `target_screw_y` | float | Coordenada Y do parafuso |

### 5. Exemplo Prático

```xml
<!-- Parafuso controlador -->
<object id="2" type="screw_flathead" x="480" y="200"/>

<!-- Portão controlado -->
<object id="3" type="gate" x="288" y="320" width="32" height="96">
 <properties>
  <property name="target_screw_x" type="float" value="480"/>
  <property name="target_screw_y" type="float" value="200"/>
 </properties>
</object>
```

## Configurações Avançadas

### Tipos de Parafuso
- `screw_flathead`: Destruído com ataque cortante
- `screw_phillips`: Destruído com ataque perfurante

### Dimensões do Portão
- **width**: Largura em pixels (padrão: 32)
- **height**: Altura em pixels (padrão: 64)
- Valores maiores criam portões maiores

### Posicionamento Estratégico

#### Bloqueio de Passagem
```
[Parede] [Portão] [Parede]
```
Coloque o portão entre paredes para forçar o player a encontrar o parafuso.

#### Acesso a Áreas Secretas
```
[Plataforma]
    [Portão]
[Área Secreta]
```
Use portões verticais para bloquear acesso a áreas especiais.

#### Puzzle de Múltiplos Parafusos
```
[Parafuso 1] → [Portão 1] → [Parafuso 2] → [Portão 2] → [Recompensa]
```

## Exemplo de Arquivo TMX Completo

```xml
<?xml version="1.0" encoding="UTF-8"?>
<map version="1.10" orientation="orthogonal" width="20" height="15" tilewidth="32" tileheight="32">
 <objectgroup name="spawns">
  <!-- Player spawn -->
  <object id="1" type="player_spawn" x="96" y="384"/>
  
  <!-- Puzzle: Parafuso escondido -->
  <object id="2" type="screw_flathead" x="480" y="200"/>
  
  <!-- Portão bloqueando passagem -->
  <object id="3" type="gate" x="288" y="320" width="32" height="96">
   <properties>
    <property name="target_screw_x" type="float" value="480"/>
    <property name="target_screw_y" type="float" value="200"/>
   </properties>
  </object>
 </objectgroup>
</map>
```

## Dicas de Design

### 1. **Feedback Visual**
- Coloque o parafuso em local visível ou use dicas visuais
- Considere a trajetória do player

### 2. **Dificuldade Progressiva**
- Primeiro nível: Parafuso fácil de ver e alcançar
- Níveis avançados: Parafusos em locais desafiadores

### 3. **Múltiplos Portões**
- Cada portão precisa de seu próprio parafuso
- Coordenadas devem ser exatas (tolerância de 20 pixels)

### 4. **Teste no Jogo**
- Sempre teste se o portão abre quando o parafuso é destruído
- Verifique se o portão fecha quando o parafuso reaparece

## Verificação de Erros Comuns

❌ **Problema**: Portão não abre
- ✅ **Solução**: Verificar se as coordenadas do parafuso estão corretas

❌ **Problema**: Parafuso não encontrado
- ✅ **Solução**: Tolerância é de 20 pixels, ajustar coordenadas

❌ **Problema**: Portão não aparece
- ✅ **Solução**: Verificar se o tipo é exatamente "gate"

❌ **Problema**: Colisão estranha
- ✅ **Solução**: Verificar dimensões width/height do portão
