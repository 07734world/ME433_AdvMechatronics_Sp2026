#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

char testLetter[]={0x7f, 0x09, 0x09, 0x09, 0x01};

//char testLetter[5][1]={0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001};
//char testLetter[8]={0b01010101};

//int n; int i; int x; int y; //char letter[8]; int subpixel;
void drawLetter(int x, int y, int messageLetter) {
    for(int i=0; i<=4; i++) {
            for(int j=0; j<=7; j++) {
                //subpixel=n%2;
                char letter;
                letter=ASCII[messageLetter-0x20][i];
                printf("ASCIIletters=%x\n",ASCII[messageLetter-0x20][i]);
                //char shiftLetter=(letter[i])>>j;
                int subpixel=(letter>>j)&0b1;
                ssd1306_drawPixel(x+i,y+j,subpixel);
                printf("i=%i, j=%i, letter%x, subpixel=%i\n", i, j, letter, subpixel);    
            }
        }
    }

char message[23];
//int i="hi";

void drawMessage(int x, int y, char message[30]){
    char displayMessage[20];
    sprintf(displayMessage, "%s", message);
    printf("sprinf says: %s\n", displayMessage);
    int k=0;
    while (displayMessage[k] != 0){
        printf("displayMessage=%x\n", displayMessage[k]);
        drawLetter(x+k*5,y,displayMessage[k]);
        k++;
    }
}
    

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
       

    while (true) {
        // Heartbeat
        gpio_init(15);
        gpio_set_dir(15, GPIO_OUT);

        gpio_put(15, 1);
        sleep_ms(25/8);
        gpio_put(15, 0);
        /*sleep_ms(10/8);
        gpio_put(15, 1);
        sleep_ms(10/8);
        gpio_put(15, 0);
        sleep_ms(10);
        */

        //ssd1306_clear();
        ssd1306_update();
        sleep_ms(10);
        unsigned int t = to_us_since_boot(get_absolute_time());
        //ssd1306_drawPixel(1,1,1);
        //drawLetter(1,1,testLetter);
        sprintf(message,"%s","HELLO.hello.HELLO.hello.HELLO");
        drawMessage(1,1,message);
        drawMessage(1,8,"07734.07734.07734.07734.07734");
        drawMessage(1,16,"012345678901234567890123456");
        //drawMessage(1,24,"---------------------------");
  

        //========================
        // ADC0 voltage readout
        gpio_init(26);
        gpio_set_dir(26, GPIO_IN);
        
        adc_init();
        adc_gpio_init(26); // pin GP26 is pin ADC0
        adc_select_input(0); // sample from ADC0
        
        uint16_t ADC0volts = adc_read();
        
        sprintf(message,"ADC0 [V]=~%.2f",(float)ADC0volts/4095*3.3);
        drawMessage(55,24, message);
        ssd1306_update();


        //=========================
        // FPS
        float timeDiff = to_us_since_boot(get_absolute_time())-t;
        float FPS=1/(timeDiff/1000000);  
        sprintf(message,"FPS~%.4f",FPS);
        drawMessage(1,24, message);

    }
}
