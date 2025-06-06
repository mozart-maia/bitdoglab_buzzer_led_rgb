#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "stdio.h"

// Configuração do pino do buzzer
#define BUZZER_PIN 21

#define FIRE_PIN 8
#define BUTTON_B 6

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

// led rgb central
#define LEDB 12
#define LEDG 11
#define LEDR 13

#define DO 13200
#define RE 14850
#define MI 16500
#define FA 17595
#define SOL 19800
#define LA 22004
#define SI 24750
#define DO2 26400

int melody[] = {
  DO, DO
};

int durations[] = {
  500, 500
};

void setup_led() {
    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, true);
    gpio_set_dir(LEDG, true);
    gpio_set_dir(LEDB, true);
}


void led_rgb_put(bool r, bool g, bool b) {
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}

void play_tone(uint pin, uint frequency, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(pin, 0); // Desliga o som após a duração
    sleep_ms(50); // Pausa entre notas
}

void play_music() {
    // play_up_sound();
    printf("playing music...");
    const int pin = 21;
    led_rgb_put(false, false, true);
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
      if (melody[i] == 0) {
        sleep_ms(durations[i]);
      } else {
        play_tone(pin, melody[i], durations[i]);
      }
    }
    led_rgb_put(false, false, false);
    
}


int main() {
    // Inicializar o sistema de saída padrão
    stdio_init_all();
    setup_led();

    // usando o botao b para testar
    // gpio_init(BUTTON_B);
    // gpio_set_dir(BUTTON_B, GPIO_IN);
    // gpio_pull_up(BUTTON_B);
    //testando leds
    led_rgb_put(true, false, false);
    sleep_ms(500);
    led_rgb_put(false, true, false);
    sleep_ms(500);
    led_rgb_put(false, false, true);
    sleep_ms(500);
    led_rgb_put(false, false, false);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // inicializar pino gpio usado para sinal
    gpio_set_dir(FIRE_PIN, GPIO_IN);

    // Loop infinito
    while (true) {
      int is_fire = gpio_get(FIRE_PIN);
      // int state_b = gpio_get(BUTTON_B);
      int state_b = 1;
      // printf("state button: %d \n", state_b);
      printf("state fire pin: %d \n", is_fire);
      
      //  
       if (state_b == 0 || is_fire == 1){
          play_music();                    
       }
       
       sleep_ms(100); 
    }
    return 0;
}