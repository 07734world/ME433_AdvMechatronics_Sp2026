/*

    JS Custom Written Functions

 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// hearbeat macros
#define GPIO_PIN 15
#define DELAY 250

void heartbeat(void){
    gpio_init(GPIO_PIN);
    gpio_set_dir(GPIO_PIN, GPIO_OUT);

    gpio_put(GPIO_PIN, 1);
    sleep_ms(DELAY);
    gpio_put(GPIO_PIN, 0);
    sleep_ms(DELAY);
}

void GraphOnCLI(float inputValue, float maxValue){
        char graphBar[]=" |                                                 ";      
        int graphBarLength = sizeof(graphBar);
        int markPosition;
        if((inputValue/maxValue) > 1){
            /*markPosition=graphBarLength;
            char saturatedText[]="<!> SATURATED";
            for(int i=0; i<sizeof(saturatedText); i++){
                graphBar[markPosition-(sizeof(saturatedText)-i)]=saturatedText[i];
            }*/
            markPosition = graphBarLength-1;
            graphBar[markPosition-3]='S';
            graphBar[markPosition-2]='A';
            graphBar[markPosition-1]='T';
        }
        else if((inputValue/maxValue) <= 1){
            markPosition = (graphBarLength-2)*(inputValue/maxValue);
            graphBar[markPosition+2]='X';        
        }
        else{
            /*char errorText[]="<GraphOnCLI ERROR>";
            for(int i=0; i<sizeof(errorText); i++){
                graphBar[markPosition-(sizeof(errorText)-i)]=errorText[i];
            }*/
            markPosition = graphBarLength-1;
            char graphBar[]="<GraphOnCLI ERROR>"; 
        }
        printf("%s\n", graphBar);
}