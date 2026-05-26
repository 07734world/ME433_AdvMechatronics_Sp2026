#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#include "heartbeat_v2.h"

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1



int main()
{
    stdio_init_all();

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_format(UART_ID,8,1,UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);
    
    //uart_set_hw_flow(UART_ID,false,false);
    /*
    int UART_IRQ = UART_ID;
    //irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);*/
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    //uart_puts(UART_ID, " Hello, UART!\n");


    
    
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
    sleep_ms(500);
    //char message[10];
    uint8_t buffer[20];
    //uint8_t ch = uart_getc(UART_ID);
    int i=1;
    int toggle=0;
   
    while (true) {
        heartbeat();

        sleep_ms(500);
    
        if (uart_is_readable(UART_ID)&&toggle==0){
            printf("UART is readable\n");
            /*for(i=1; i<sizeof(buffer); i++){
                buffer[i]=uart_getc(UART_ID);
                printf("%d",buffer[i]);
            }*/
            
          
            uart_read_blocking(UART_ID, buffer, 10);
            printf("[Pico Rx]: ");
            for(int i=1;i<strlen(buffer);i++){
                printf("%c",buffer[i]);
            }
            printf("\n");
            toggle=1;

        } else{
        //======================================================
        //  TRANSMIT
        //
        uint8_t txMessage[20];
        printf("[Pico Tx]: \n");
        fgets(txMessage, sizeof(txMessage),stdin);
        sleep_ms(1000);
        printf("Sending: %s\n",txMessage);

        uart_write_blocking(UART_ID, txMessage, sizeof(txMessage));
        uart_tx_wait_blocking(UART_ID);
        //uart_puts(UART_ID, " Hello, UART!\n");
        toggle=0;
        sleep_ms(500);
        }
        
        /*else if(!uart_is_readable(UART_ID)){
            printf("UART is NOT readable\n");
        }

        if(uart_is_enabled(UART_ID)){
            printf("UART is enabled\n");
        } else if(!uart_is_enabled(UART_ID)){
            printf("UART is NOT enabled\n");
        }
        */
        //printf("Input: \n");
        //uint8_t ch = uart_getc(UART_ID);
        //scanf("%s", &message);
        //fgets(message,sizeof(message),stdin);

        //uart_read_blocking(UART_ID, &ch, 3);
        //message[1]=uart_getc(UART_ID);
        
        //printf("UART rx: %d\n", ch);
        //sleep_ms(1000);

      

    }
}
