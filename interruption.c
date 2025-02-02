#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "interruption.pio.h"

// Definições de pinos
#define LED_RED_PIN     13     // Pino do LED vermelho
#define MATRIX_WS2812_PIN     7     // Pino de controle da matriz 5x5
#define BUTTON_A_PIN   5     // Botão para incrementar os números (Pino GPIO 5)
#define BUTTON_B_PIN   6     // Botão para decrementar os números (Pino GPIO 6)

#define DEBOUNCE_DELAY 200 // Tempo de debounce em milissegundos


volatile uint8_t current_number = 0;  // Número atual exibido
volatile bool update_num_matrix = false; // Flag utilizada para atualizar a matriz
volatile int button_value = 0;  // 1 = Incrementar, -1 = Decrementar



/**
 * @brief Rotina de serviço de interrupção para os botões
 * @param gpio Pino GPIO que gerou a interrupção
 * @param events Tipo de evento que acionou a interrupção
 * @details Manipula o debounce e registra a ação do botão
 */

void button_interruption(uint gpio, uint32_t events) {
    static uint64_t last_press = 0;
    uint64_t now = time_us_64() / 1000;  

    if (now - last_press > DEBOUNCE_DELAY) {
        if (gpio == BUTTON_A_PIN && gpio_get(BUTTON_A_PIN) == 0) { 
            button_value = 1; // Incrementar
            update_num_matrix = true;
        }
        if (gpio == BUTTON_B_PIN && gpio_get(BUTTON_B_PIN) == 0) { 
            button_value = -1; // Decrementar
            update_num_matrix = true;
        }
        //Verifica qual foi o ultimo botão pressionado
        last_press = now;
    }
}

// Defina as cores para cada número (0 a 9)
const uint32_t number_colors[10] = {
    0xFF0000, // 
    0xFFFFFF, // 
    0x00FF00, // 
    0xFFFF00, // 
    0xFF00FF, // 
    0x00FFFF, // 
    0xFFA500, // 
    0x800080, // 
    0x008000, // 
    0x800000  // 
};

/**
 * @brief Matriz de representação numérica para display 5x5
 * @details Cada número (0-9) é representado por 25 valores binários (5x5)
 *          Valor 1 = LED ligado, 0 = LED desligado
 */

const uint32_t numbers[10][25] = {
    // Número 0
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1},
    // Número 1
    {1,1,1,1,1,  0,0,1,0,0,  0,0,1,0,1,  0,1,1,0,0,  0,0,1,0,0},
    // Número 2
    {1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1},
    // Número 3
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1},
    // Número 4
    {1,0,0,0,0,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1},
    // Número 5
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1},
    // Número 6
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1},
    // Número 7
    {0,0,0,0,1,  0,1,0,0,0,  0,0,1,0,0,  0,0,0,1,0,  1,1,1,1,1},
    // Número 8
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1},
    // Número 9
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1},
};

/**
 * @brief Atualiza a matriz de LEDs com o número especificado
 * @param number Número a ser exibido (0-9)
 * @details Envia os dados para a matriz LED WS2812 usando PIO
 */

void update_led_matrix(int number, PIO pio, uint sm) {
    uint32_t color = number_colors[number]; // Obtém a cor correspondente ao número

    for (int i = 0; i < 25; i++) {
        if (numbers[number][i]) {
            pio_sm_put_blocking(pio, sm, color); // Envia a cor para o LED
        } else {
            pio_sm_put_blocking(pio, sm, 0); // Desliga o LED
        }
    }
    sleep_us(50);
}

/**
 * @brief Função principal de inicialização e loop de controle
 */

int main() {
    stdio_init_all();

    // Configuração dos botões com pull-up
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração do LED vermelho
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    // Configuração das interrupções dos botões para os tipos FALLING e RISING
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_interruption);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_interruption);

    // Inicializa o PIO para controlar os LEDs WS2812
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &interruption_program);
    interruption_program_init(pio, sm, offset, MATRIX_WS2812_PIN);

    // Exibe o número inicial na matriz
    update_led_matrix(current_number, pio, sm);

    // Loop principal
    while (true) {
        gpio_put(LED_RED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_RED_PIN, 0);
        sleep_ms(100);

        // Atualização do display se necessário
        if (update_num_matrix) {
            if (button_value == 1) {
                current_number = (current_number + 1) % 10;
            } else if (button_value == -1) {
                current_number = (current_number == 0) ? 9 : current_number - 1;
            }
            update_led_matrix(current_number,pio,sm);
            update_num_matrix = false;
        }
    }
}
