#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "hardware/irq.h"
#include "hardware/clocks.h"

#include "interruption.pio.h"

// Definições de pinos
#define LED_R_PIN     13     // Pino do LED vermelho
#define WS2812_PIN     7     // Pino de controle da matriz 5x5
#define BUTTON_A_PIN   5     // Botão para incrementar os números (Pino GPIO 5)
#define BUTTON_B_PIN   6     // Botão para decrementar os números (Pino GPIO 6)

// Constantes de configuração
#define DEBOUNCE_DELAY_MS 200  // Tempo de debounce implementado em milissegundos

// Variáveis globais voláteis
volatile uint8_t current_number = 0;   // Número atual exibido (0-9)
volatile bool update_numbers_display = false; // Flag para atualização do display
volatile int button_action = 0;        // Ação do botão: 1=Incrementar, -1=Decrementar

/**
 * @brief Matriz de representação numérica para display 5x5
 * @details Cada número (0-9) é representado por 25 valores binários (5x5)
 *          Valor 1 = LED ligado, 0 = LED desligado
 */
const uint32_t number_patterns[10][25] = {
    // Padrão para o número 0
    {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1},
    // Padrão para o número 1
    {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,1, 0,0,1,1,0, 0,0,1,0,0},
    // Padrão para o número 2
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1},
    // Padrão para o número 3
    {1,1,1,1,1, 0,0,0,0,1, 0,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1},
    // Padrão para o número 4
    {1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1},
    // Padrão para o número 5
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1},
    // Padrão para o número 6
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1},
    // Padrão para o número 7
    {0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,0,0,0,0, 1,1,1,1,1},
    // Padrão para o número 8
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1},
    // Padrão para o número 9
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}
};

/**
 * @brief Atualiza a matriz de LEDs com o número especificado
 * @param number Número a ser exibido (0-9)
 * @details Envia os dados para a matriz LED WS2812 usando PIO
 */

void update_led_matrix(uint8_t number) {
    for (int i = 0; i < 25; i++) {
        uint8_t green = number_patterns[number][i] ? 255 : 0;
        uint8_t red = number_patterns[number][i] ? 255 : 0;
        uint8_t blue = 0;
        uint32_t color = (green << 16) | (red << 8) | blue;
        pio_sm_put_blocking(pio0, 0, color);
    }
    sleep_us(50);  // Pequena pausa para estabilização
}

/**
 * @brief Rotina de serviço de interrupção para os botões
 * @param gpio Pino GPIO que gerou a interrupção
 * @param events Tipo de evento que acionou a interrupção
 * @details Manipula o debounce e registra a ação do botão
 */

void button_interrupt(uint gpio, uint32_t events) {
    static uint64_t last_interrupt_time = 0;
    uint64_t current_time = time_us_64() / 1000;

    // Verificação de debounce
    if (current_time - last_interrupt_time > DEBOUNCE_DELAY_MS) {
        if (gpio == BUTTON_A_PIN && !gpio_get(BUTTON_A_PIN)) {
            button_action = 1;
            update_numbers_display = true;
            printf("O Botão A pressionado\n");
        }
        if (gpio == BUTTON_B_PIN && !gpio_get(BUTTON_B_PIN)) {
            button_action = -1;
            update_numbers_display = true;
            printf("O Botão B pressionado\n");
        }
        last_interrupt_time = current_time;
    }
}

/**
 * @brief Função principal de inicialização e loop de controle
 */
int main() {
    stdio_init_all();

    // Configuração do LED embutido
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);

    // Configuração dos botões com resistores pull-up
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração de interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, 
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_interrupt);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, 
        GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_interrupt);

    // Inicialização do controlador PIO para os LEDs
    PIO pio = pio0;
    uint state_machine = 0;
    uint program_offset = pio_add_program(pio, &inter_program);
    inter_program_init(pio, state_machine, program_offset, WS2812_PIN);

    // Exibição inicial do número 0
    update_led_matrix(current_number);

    // Loop principal de operação
    while (true) {
        // Piscar LED embutido (indicador de atividade)
        gpio_put(LED_R_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_R_PIN, 0);
        sleep_ms(100);

        // Atualização do display se necessário
        if (update_numbers_display) {
            if (button_action == 1) {
                current_number = (current_number + 1) % 10;
                printf("Número atual: %d\n", current_number);
            } else if (button_action == -1) {
                current_number = (current_number == 0) ? 9 : current_number - 1;
                printf("Número atual: %d\n", current_number);
            }
            update_led_matrix(current_number);
            update_numbers_display = false;
        }
    }
}