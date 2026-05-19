#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_DAC   17
#define PIN_CS_RAM   14
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
    float o=thetaRad-(powerofJS(thetaRad,3)/factorialJS(3))+(powerofJS(thetaRad,5)/factorialJS(5))
        -(powerofJS(thetaRad,7)/factorialJS(7))+(powerofJS(thetaRad,9)/factorialJS(9))-(powerofJS(thetaRad,11)/factorialJS(11));
    o=-o;
    return o;
}

void writeDAC16b (uint16_t datum){
    //uint8_t data[2]={((datum<<8 & 0b1111111100000000)>>8),(datum>>8)};
    uint8_t data[2]={(datum>>8),((datum<<8 & 0b1111111100000000)>>8)};
    printf("[0]: %b, [1]: %b",data[0],data[1]);
        
    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_DAC);
}

void spi_ram_init(){
    uint8_t data[2];
    int len = 2;
    data[0] = 0b00000001;
    data[1] = 0b01000000; //sequential mode
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_write(uint16_t addr, uint8_t * data, int len){
    uint8_t packet[5];
    packet[0]=0b00000010; //instr, write
    packet[1]=addr>>8; //addr - pt1
    packet[2]=addr&0xFF; //addr - pt2
    packet[3]=data[0];
    packet[4]=data[1];

    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, packet, 5);
    cs_deselect(PIN_CS_RAM);
}

void ram_write_sine(){
//1000x data point for sin wave and store as 16-bit unsigned
}

void spi_ram_read(uint16_t addr, uint8_t * data, int len){
    uint8_t packet[5];
    packet[0]=0b00000011; //instr, read
    packet[1]=addr>>8; //addr - pt1
    packet[2]=addr&0xFF; //addr - pt2
    packet[3]=0;
    packet[4]=0;

    uint8_t dst[5]; //incoming data packet
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, packet, dst, 5);
    cs_deselect(PIN_CS_RAM);

    data[0]=dst[3];
    data[1]=dst[4];
}

void update_dac_from_ram(int i){
    uint8_t data[2];
    spi_ram_read(i, data, 2);

    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_CS_DAC);
}


int main()
{
    stdio_init_all();
    sleep_ms(1000);
    

    //===========================================================================
    // Calculate Sin wave @ 1Hz
    
    int stepNumber=10;
    uint16_t sinStorage[stepNumber]; 
    float thetaRad=0; //init

    for(int i=0;i<stepNumber;i++){
        uint16_t sinData=i;//(1023.*0.5*(1.+sinJS(thetaRad)));
        thetaRad=thetaRad+((2*PI)/(stepNumber));
        /*if (thetaRad>(2*PI)){
            break;
        }*/
        sinStorage[i]=sinData;
         
        //printf("inside %f\n",sinStorage[i]);
    }
    
     
    

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS_RAM,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);
    gpio_put(PIN_CS_RAM, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // set RAM into sequential mode
    spi_ram_init();

    char graph[stepNumber]; int barCharLength=64;
    while (true) {
        //printf("%s/n",graph);
        printf("\n\n");
        for(int i=0; i<stepNumber; i++){
            // Print data
            printf("data: %.4u, %.16b, ", sinStorage[i], sinStorage[i]);

            // Graph data on CLI
            char graphBar[barCharLength]; 
            graphBar[0]='|';
            int markPosition = barCharLength*(sinStorage[i]/1);
            graphBar[markPosition]='*';        
            printf("%s\n",graphBar);
            graphBar[markPosition]=' ';


            //printf("uint32: %u\n", sinStorageUI[i]);
            //printf("uint16: %u\n\n", sinStorageI[i]);
        }
        sleep_ms(2000);
    }
}
