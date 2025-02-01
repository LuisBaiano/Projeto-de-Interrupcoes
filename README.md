# Projeto de Display Numérico com Matriz LED 5x5 em Raspberry Pi Pico W


## 📖 Descrição
Projeto de controle de uma matriz LED 5x5 WS2812 usando Raspberry Pi Pico. O sistema permite exibir números de 0 a 9 com controle por botões físicos, incluindo:
- Incremento/decremento numérico
- Visualização em tempo real
- Debounce de botões
- Feedback visual com LED embutido

## ✨ Funcionalidades Principais
- **Controle por Botões**
  - Botão A (GP5): Incrementa número (0→9→0)
  - Botão B (GP6): Decrementa número (9→0→9)
- **Matriz LED Inteligente**
  - Padrões numéricos pré-definidos
  - Atualização em tempo real
  - Controle por protocolo WS2812
- **Sistema Resiliente**
  - Debounce de 200ms para botões
  - Proteção contra bouncing
  - Auto-recovery em caso de falhas

## 🛠 Componentes Necessários
| Componente               | Especificações               | Quantidade |
|--------------------------|-------------------------------|------------|
| Raspberry Pi Pico        | Microcontrolador RP2040       | 1          |
| Matriz LED WS2812 5x5    | Endereçável individualmente   | 1          |
| Botões tactiles          | Momentâneos NO                | 2          |
| Resistores               | 10KΩ (pull-down)              | 2          |
| Protoboard               | 400 pontos                    | 1          |
| Jumpers                  | Macho-macho                   | 10+        |

## ⚙️ Configuração do Hardware
```plaintext
+-------------------+-------------------------------+
| Componente        | Conexão Pico                 |
+-------------------+-------------------------------+
| WS2812 Data In    | GP7 (WS2812_PIN)             |
| Botão A           | GP5 (BUTTON_A_PIN)           |
| Botão B           | GP6 (BUTTON_B_PIN)           |
| LED Vermelho      | GP13 (LED embutido)          |
+-------------------+-------------------------------+
