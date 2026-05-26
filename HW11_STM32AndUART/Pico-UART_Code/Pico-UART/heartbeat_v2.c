/*

    JS Hearbeat via GPIO

 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
 
#define GPIO_PIN 15
#define DELAY 250

void heartbeat(){
    gpio_init(GPIO_PIN);
    gpio_set_dir(GPIO_PIN, GPIO_OUT);

    gpio_put(GPIO_PIN, 1);
    sleep_ms(DELAY);
    gpio_put(GPIO_PIN, 0);
    sleep_ms(DELAY);
}
