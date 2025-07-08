# Guia de Uso da Classe Gate

## Visão Geral
A classe `Gate` implementa portões que são controlados por parafusos. Quando um parafuso específico é destruído, o portão se abre automaticamente. Se o parafuso for regeado (respawn), o portão se fecha novamente.

## Funcionalidades

### Estados do Portão
- **CLOSED**: Portão fechado (bloqueia passagem)
- **OPENING**: Portão abrindo (animação)
- **OPEN**: Portão aberto (não bloqueia passagem)
- **CLOSING**: Portão fechando (animação)

### Sistema de Controle
- Cada portão pode ser vinculado a um parafuso específico
- Quando o parafuso é destruído, o portão abre
- Se o parafuso reaparecer (respawn), o portão fecha
- Animação suave de abertura/fechamento

## Como Usar no Editor Tiled

### 1. Criando um Portão no TMX

```xml
<object id="10" name="gate1" type="gate" x="400" y="300" width="32" height="64">
  <properties>
    <property name="target_screw_x" type="float" value="500"/>
    <property name="target_screw_y" type="float" value="250"/>
  </properties>
</object>
```

### 2. Criando o Parafuso Controlador

```xml
<object id="11" name="control_screw" type="screw_flathead" x="500" y="250"/>
```

### 3. Propriedades Obrigatórias

- **type**: Deve ser "gate"
- **x, y**: Posição do portão no mapa
- **width, height**: Dimensões do portão (opcional, padrão: 32x64)

### 4. Propriedades Customizadas

- **target_screw_x**: Coordenada X do parafuso controlador
- **target_screw_y**: Coordenada Y do parafuso controlador

## Exemplo de Uso Prático

### Cenário: Portão que bloqueia caminho

1. **Coloque um portão** na posição onde você quer bloquear o caminho
2. **Coloque um parafuso** em algum lugar do nível (pode ser difícil de alcançar)
3. **Configure as coordenadas** do parafuso nas propriedades do portão
4. **O player** deve encontrar e destruir o parafuso para abrir o portão

### Exemplo de Configuração TMX

```xml
<!-- Portão bloqueando uma passagem -->
<object id="20" type="gate" x="800" y="400" width="32" height="96">
  <properties>
    <property name="target_screw_x" type="float" value="1200"/>
    <property name="target_screw_y" type="float" value="200"/>
  </properties>
</object>

<!-- Parafuso em local elevado/difícil -->
<object id="21" type="screw_phillips" x="1200" y="200"/>
```

## Funcionalidades Técnicas

### Colisão
- Portões fechados bloqueiam o movimento do player
- Portões abertos não têm colisão
- Durante a animação, a colisão é parcial (baseada na parte visível)

### Renderização
- Animação de "subida" quando abre
- Sprite desaparece gradualmente
- Renderização baseada no estado atual

### Performance
- Sistema otimizado para verificar apenas parafusos próximos
- Animações fluidas com deltaTime
- Memória eficiente com smart pointers

## Métodos Públicos Úteis

```cpp
// Verificar estado
bool isOpen() const;
bool isClosed() const;
bool isMoving() const;

// Controle manual (para debug/eventos especiais)
void forceOpen();
void forceClose();

// Configurações
void setTargetScrew(Screw* screw);
void setOpeningSpeed(float speed);
```

## Integração com o GameWorld

A classe está totalmente integrada:
- ✅ Carregamento automático do TMX
- ✅ Atualização no loop principal
- ✅ Renderização com câmera
- ✅ Sistema de colisão
- ✅ Limpeza automática de memória

## Próximas Funcionalidades (Futuras)

- [ ] Suporte a múltiplos parafusos por portão
- [ ] Sprites customizados para estados diferentes
- [ ] Sons de abertura/fechamento
- [ ] Temporizadores de abertura automática
- [ ] Portões que se abrem com chaves/itens
