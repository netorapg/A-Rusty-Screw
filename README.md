# Tarefas a Fazer

## Estrutura e Organização
* [ ] Criar um namespace para organizar melhor o código.
* [ ] Criar uma classe para manipulação de sprites e animações.
* [ ] Melhorar a hierarquia de classes, garantindo que objetos estáticos e dinâmicos compartilhem uma mesma estrutura base.
* [ ] Criar uma função centralizada para renderizar e implementar a câmera nela.

## Vetores e Posições
* [ ] Substituir o uso de `x` e `y` separados por vetores/pontos.
* [ ] Sugestão: `#include "my-lib/math-vector.h"`
* [ ] `using Vector = Mylib::Math::Vector2f;`
* [ ] `using Point = Vector;`
* [ ] Alterar `int width, int height` para `float` para maior precisão.
* [ ] Fazer a câmera trabalhar com `float` ao invés de `int`.
* [ ] Usar tempo para atualizar posição e velocidade, garantindo um movimento mais suave e consistente.

## Boas Práticas
* [ ] Programar `getter` e `setter` diretamente no `.h` como funções `inline` para melhor desempenho.

