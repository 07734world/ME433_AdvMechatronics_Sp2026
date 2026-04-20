#include <stdio.h>
#include "pico/stdlib.h"

void heartbeat(){
    stdio_init_all();

    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);

    gpio_put(15, 1);
    sleep_ms(1000/8);
    gpio_put(15, 0);
    sleep_ms(1000/8);
    gpio_put(15, 1);
    sleep_ms(1000/8);
    gpio_put(15, 0);
    sleep_ms(5000);
}

int main()
{
    heartbeat();
    
    /*
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        gpio_init(15);
        gpio_set_dir(15, GPIO_OUT);

        gpio_put(15, 1);
        sleep_ms(1000/8);
        gpio_put(15, 0);
        sleep_ms(1000/8);
        gpio_put(15, 1);
        sleep_ms(1000/8);
        gpio_put(15, 0);
        sleep_ms(1000);
    }
    */
}
