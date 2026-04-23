#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// ****** LED HEARTBEAT ******
// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}
// *****************************


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5



int main()
{
    // *** LED Heartbeat ***
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    /*while (true) {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
    }*/
    // ******


    //I2c code...
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    //static uint8_t ADDR=0b0100000;
    
    char ADDR = 0b0100000;

    char bufRW[2];
    bufRW[0] = 0x00;
    bufRW[1] = 0b00000001;

    char bufREAD[2];
    bufREAD[0]=0x09;
    bufREAD[1]=0b00000001;

    char bufSET[2];
    bufSET[0]=0x0A;
    bufSET[1]=0b00000000;
    
    char readDATA;

    while (true) {
        //printf("Hello, world!\n");
        //sleep_ms(1000);
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS/8);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS/8);
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS/8);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
        
        printf("**************************\n");
        printf("%i, %i\n",bufRW[0],bufRW[1]);
        printf("%i, %i\n",bufSET[0],bufSET[1]);

        i2c_write_blocking(i2c_default, ADDR, bufRW, 2, false);
        i2c_write_blocking(i2c_default, ADDR, bufSET, 2, false);

      
        i2c_write_blocking(i2c_default, ADDR, &bufREAD[0], 1, true);  // true to keep host control of bus
        i2c_read_blocking(i2c_default, ADDR, &readDATA, 1, false);  // false - finished with bus
       
        printf("Read DATA: %b \n", readDATA);
        readDATA=readDATA<<7;
        printf("Read DATA: %b \n", readDATA);
        //char buttonPRESS=readDATA & 0b00000001;     
        //printf("Button PRESS: %b \n", buttonPRESS);

        if (readDATA==0){
            printf("ON!\n");
            bufSET[1]=0b10000000;
            i2c_write_blocking(i2c_default, ADDR, bufSET, 2, false);
        }
        
        else{
            printf("off :(\n");
            bufSET[1]=0b00000000;
            i2c_write_blocking(i2c_default, ADDR, bufSET, 2, false);
            }
            
    }


}
