#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
//#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5



//int n; int i; int x; int y; //char letter[8]; int subpixel;
void drawLetter(int x, int y, int messageLetter) {
    for(int i=0; i<=4; i++) {
            for(int j=0; j<=7; j++) {
                //subpixel=n%2;
                char letter;
                letter=ASCII[messageLetter-0x20][i];
                //printf("ASCIIletters=%x\n",ASCII[messageLetter-0x20][i]);
                //char shiftLetter=(letter[i])>>j;
                int subpixel=(letter>>j)&0b1;
                ssd1306_drawPixel(x+i,y+j,subpixel);
                //printf("i=%i, j=%i, letter%x, subpixel=%i\n", i, j, letter, subpixel);    
            }
        }
    }

//char message[23];
//int i="hi";

void drawMessage(int x, int y, char message[30]){
    char displayMessage[20];
    sprintf(displayMessage, "%s", message);
    //printf("sprintf says: %s\n", displayMessage);
    int k=0;
    while (displayMessage[k] != 0){
        //printf("displayMessage=%x\n", displayMessage[k]);
        drawLetter(x+k*5,y,displayMessage[k]);
        k++;
    }
}
    
