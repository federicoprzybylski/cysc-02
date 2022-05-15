#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

int main() {
  float conversion_factor = 3.3 / (4095);
  stdio_init_all();
  adc_init();
  adc_gpio_init(26);
  adc_gpio_init(27);
  gpio_set_function(9, GPIO_FUNC_PWM);
  gpio_set_function(8, GPIO_FUNC_PWM);

  uint8_t slice_num = pwm_gpio_to_slice_num(9);
  pwm_config config = pwm_get_default_config();
  pwm_init(slice_num, &config, true);
  while (true) {
    adc_select_input(0);
    uint16_t result = adc_read();
    float r = result * conversion_factor;
    int Ts = (r*100);
    adc_select_input(1);
    uint16_t result1 = adc_read();
    float r1 = result1 * conversion_factor;
    int Tp = (r1*35)/3.3;

    int16_t dif = Tp - Ts;
    if(dif > 10) {
        dif = 10;
    }
    else if(dif < -10) {
        dif = -10;
    }
    uint16_t cd = dif * 0xffff / 10;

    printf("Temperatura: %d\n", Ts);
    printf("Pote: %d\n", Tp);

    if (Tp > Ts){
      pwm_set_gpio_level(8, 0);
      pwm_set_gpio_level(9, cd);
    }
    if (Tp < Ts){
      pwm_set_gpio_level(9, 0);
      pwm_set_gpio_level(8, -cd);
    }
    sleep_ms(250);
  }
}