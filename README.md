# Projeto de Display Numérico com Matriz LED 5x5 em Raspberry Pi Pico W para estudo de interrupções

**Autor: Luis Felipe Pereira de Carvalho**

**Técnico em Analises e Desenvolvimento de Sistemas - SENAI**

**Graduando em Bacharelado de Computação - UEFS**

## Índice

1. [Objetivos](#objetivos)
2. [Descrição](#descri%C3%A7%C3%A3o)
3. [Funcionalidades](#funcionalidades)
4. [Requisitos do Projeto](#requisitos-do-projeto)
5. [Como Executar o Programa](#como-executar-o-programa)
6. [Estrutura do Código](#estrutura-do-c%C3%B3digo)
7. [Referências](#refer%C3%AAncias)
8. [Demonstrativo em Vídeo](#demonstrativo-em-v%C3%ADdeo)

## Objetivos

Este projeto tem como objetivo implementar um contador numérico utilizando a Raspberry Pi Pico (RP2040) e uma matriz de LEDs WS2812. O usuário pode aumentar ou diminuir os números exibidos na matriz através de botões físicos de modo a estudar as interrupções de ciclos em um sistema.

## Descrição

O sistema utiliza a Raspberry Pi Pico para controlar uma matriz 5x5 de LEDs WS2812, exibindo números de 0 a 9. Dois botões permitem a navegação pelos números, e um LED vermelho indica o funcionamento do sistema. A comunicação com os LEDs é feita via PIO (Programmable Input/Output), garantindo uma atualização eficiente da matriz.

## Funcionalidades principais

* Exibição de números de 0 a 9 em uma matriz 5x5 de LEDs WS2812.
* Controle de incremento e decremento numérico via botões físicos.
* Indicação do funcionamento do sistema através de um LED vermelho.
* Implementação de debounce para evitar leituras erradas dos botões.
* Utilização de PIO para comunicação eficiente com a matriz de LEDs.

## Requisitos do Projeto

### Hardware

* Raspberry Pi Pico W (RP2040)
* Matriz de LEDs WS2812 (5x5)
* Botões físicos (2 unidades)
* LED vermelho
* Resistores de pull-up para os botões
* Fonte de alimentação compatível

### Software

* SDK do Raspberry Pi Pico w
* Biblioteca standard `pico/stdlib.h`
* Biblioteca de controle de hardware `hardware/pio.h`, `hardware/gpio.h`, `hardware/clocks.h`, `hardware/irq.h`
* Compilador C/C++ (GCC ARM)
* Ferramenta `CMake`

## Como Executar o Programa

### 1. Configuração do Ambiente

* Instale o SDK do Raspberry Pi Pico seguindo a documentação oficial.
* Configure a ferramenta de build `CMake` e o compilador ARM.

### 2. Compilação do Código

* Clone o repositório ou copie o código para um diretório local.
* No terminal, navegue até o diretório do projeto e execute:
  ```sh
  mkdir build && cd build
  cmake ..
  make
  ```

### 3. Upload para a Raspberry Pi Pico W

* Conecte a Raspberry Pi Pico ao computador enquanto segura o botão `BOOTSEL`.
* Copie o arquivo `.uf2` gerado para a unidade que aparecerá no sistema.
* A Pico será reiniciada automaticamente e executará o código.

## Estrutura do Código

* **Definição de pinos:** Configura os pinos GPIO para LEDs e botões.
* **Rotina de interrupção:** Gerencia o acionamento dos botões com debounce.
* **Tabela de cores:** Define cores para cada número exibido.
* **Matriz de números:** Representação dos números na matriz 5x5.
* **Função `update_led_matrix()`:** Atualiza a matriz com o número correspondente.
* **Função `main()`:** Configura hardware, inicializa PIO e gerencia a lógica do programa.

## Referências

* [Documentação do Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
* [SDK do RP2040](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
* [PIO (Programmable Input/Output)](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

## Demonstrativo em Vídeo

Assista ao funcionamento do projeto no seguinte link:

[Vídeo no YouTube](https://youtu.be/uOz75-XnyGc)
