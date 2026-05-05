#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
//#include "math.h" //Only use for checking custom math functions

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

//Make math functions
#define PI 3.14159265

float factorialJS(float number){
    float factorial = number;
    float factorialStep= number-1;
    for (factorialStep; factorialStep>0; factorialStep--){
        factorial=factorial*(factorialStep);
    }
    if(factorialStep==0){
        return factorial;
    }
}

float powerofJS(float number, int power){
    float output = number;
    while(power>1){
        output=output*number;
        power--;
    }
    return output;
}

float sinJS(float thetaRad){
    // Sin(theta[rad])=Opposite/Hypotenuse
    // Approximate using Taylor series approx
    thetaRad=thetaRad-PI;
    /*float thetaRad3=thetaRad*thetaRad*thetaRad;
    float thetaRad5=thetaRad3*thetaRad*thetaRad;
    float thetaRad7=thetaRad5*thetaRad*thetaRad;
    float thetaRad9=thetaRad7*thetaRad*thetaRad;
    float thetaRad11=thetaRad9*thetaRad*thetaRad;*/
    float o=thetaRad-(powerofJS(thetaRad,3)/factorialJS(3))+(powerofJS(thetaRad,5)/factorialJS(5))
        -(powerofJS(thetaRad,7)/factorialJS(7))+(powerofJS(thetaRad,9)/factorialJS(9))-(powerofJS(thetaRad,11)/factorialJS(11));
    o=-o;
    return o;
}


void writeDAC16b (uint16_t datum){
    //uint8_t data[2]={((datum<<8 & 0b1111111100000000)>>8),(datum>>8)};
    uint8_t data[2]={(datum>>8),((datum<<8 & 0b1111111100000000)>>8)};
    printf("[0]: %b, [1]: %b",data[0],data[1]);
        
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

int main()
{
    stdio_init_all();
    
    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
    

    float stepNumber=100; 
    float updatePeriodMS=10; // [ms]
    float thetaRad=0;
    float tri1Hz=0; int triDir=1;
    while (true) {
       
        //==================== SIN 2Hz ====================
        float sin2Hz=sinJS(thetaRad);
        thetaRad=thetaRad+((2*PI)/(stepNumber/2));
        if (thetaRad>(2*PI)){
            thetaRad=0;
        }
        //printf("sin2Hz=%f,thetarad=%f\n",sin2Hz,thetaRad);
        
        /*
        for(thetaRad=0; thetaRad<(2*PI); thetaRad=thetaRad+((2*PI)/stepNumber)){
            float o=sinJS(thetaRad);
            
            //float omath=sin(thetaRad);
            //printf("thetaRad=%f, o_value=%f, omath=%f, error=%f\n",thetaRad, o, omath, (o-omath));
            //float fac=factorialJS(3);
            //printf("factorial=%f\n",fac);
        }*/
        
        //==================== TRIANGLE 1Hz ====================
        if(tri1Hz+triDir*(1/(stepNumber/2)) < 0){
            triDir=1;
        }
        if(tri1Hz+triDir*(1/(stepNumber/2)) > 1){
            triDir=-1;
        }
        tri1Hz=tri1Hz+triDir*(1/(stepNumber/2));
        printf("tri1Hz=%f\n", tri1Hz);

        if(tri1Hz>=0.99){
            printf("*************************** TRI ***************************\n");
        }
        if(sin2Hz>=0.99){
            printf("****** SIN ******\n");
        }
        printf("*\n");
            


        //==================== SPI OUTPUT TO DAC ==================== 
        uint16_t sin2HzDAC=1023*(0.5*sin2Hz+0.5); //sin2HzDAC<<6;
        uint16_t tri1HzDAC=1023*tri1Hz; //tri1HzDAC<<6;
        printf("sin=%d, tri=%b ~%d\n", sin2HzDAC, tri1HzDAC, tri1HzDAC);
        //uint16_t sin2HzDAC=1023; uint16_t tri1HzDAC=1023;
        uint16_t outA=0b0111<<10; outA=outA|sin2HzDAC; outA=outA<<2;
        uint16_t outB=0b1111<<10; 
        printf("outB=0b1111<<10: %b\n",outB);
        outB=outB|tri1HzDAC;
        printf("outB=outB|tri1HzDAC: %b\n",outB);
        outB=outB<<2;
        printf("outB=outB<<2: %b\n",outB);
        //printf("outA=%d, sin=%d\noutB=%d, tri=%d\n",outA,sin2HzDAC,outB,tri1HzDAC);
        
        //int sinData[2]={((outA<<8 & 0b1111111100000000)>>8),(outA>>8)};
        //printf("sinData[0]=%b, sinData[1]=%b\n",sinData[0], sinData[1]);

        writeDAC16b(outA); //SIN
        writeDAC16b(outB); //TRI

        sleep_ms(updatePeriodMS);
    }
}
