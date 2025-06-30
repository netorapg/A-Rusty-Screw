# Guia de Escala Visual do Boss Punktauro

## Como Funciona

O boss Punktauro agora possui um sistema de escala visual que permite aumentar seu tamanho sem modificar a arte original. Isso é feito através da variável `mVisualScale` na classe `Punktauro`.

## Ajustando a Escala

### Localização do Código
Arquivo: `src/Punktauro.cpp`
Linha: ~48 (no construtor)

```cpp
// Configurar escala visual (pode ser ajustada para fazer o boss parecer maior)
// 1.0f = tamanho original, 1.5f = 50% maior, 2.0f = 100% maior (dobro do tamanho)
mVisualScale = 1.5f; // 50% maior que o tamanho original
```

### Valores Recomendados

- **1.0f** - Tamanho original da sprite
- **1.5f** - 50% maior (padrão atual)
- **2.0f** - Dobro do tamanho (100% maior)
- **2.5f** - 2.5x o tamanho original
- **3.0f** - 3x o tamanho original

### Como Modificar

1. Abra o arquivo `src/Punktauro.cpp`
2. Procure pela linha com `mVisualScale = 1.5f;`
3. Altere o valor para o desejado (ex: `mVisualScale = 2.0f;` para dobrar o tamanho)
4. Recompile o projeto com `make`

## O Que é Afetado pela Escala

✅ **Sprite visual** - O boss aparece maior na tela
✅ **Hurtbox do corpo** - Área onde o boss pode ser atingido (corpo todo)
✅ **Hurtbox da cabeça** - Área específica da cabeça onde o boss recebe dano
✅ **Attack hitbox** - Área de ataque do boss
✅ **Hitboxes de debug** - Retângulos coloridos para debug

❌ **Posição lógica** - A posição base do boss permanece a mesma
❌ **Arquivos de sprite** - Nenhuma modificação na arte original

## Dicas

- **Para um boss mais imponente**: Use valores entre 1.8f e 2.5f
- **Para testes**: Use 2.0f para facilitar visualização das mudanças
- **Para performance**: Evite valores muito altos (acima de 3.0f)
- **Para balanceamento**: Lembre-se que hitboxes maiores facilitam acertar o boss

## Debug Visual

Para ver as hitboxes do boss escalado:
1. Execute o jogo
2. Pressione a tecla de debug (se configurada)
3. Observe os retângulos:
   - **Verde**: Hurtbox do corpo completo
   - **Amarelo**: Hurtbox específica da cabeça
   - **Vermelho**: Attack hitbox (quando atacando)

## Implementação Técnica

O sistema funciona através de:
1. **Renderização escalada**: Usa `SDL_RenderCopyEx` com retângulo de destino maior
2. **Ajuste de posição**: Centraliza a sprite escalada na posição original
3. **Hitboxes proporcionais**: Calcula hitboxes baseadas no tamanho escalado
4. **Compensação de offset**: Ajusta posições das hitboxes para alinhamento correto

Este sistema mantém a compatibilidade com todos os sistemas existentes do jogo!
