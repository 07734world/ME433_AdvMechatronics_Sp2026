#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h" // CMakeLists.txt must have hardware_pwm in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries
#include "JScustomFunctions.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_INA219_ADDR 0b1000000 

#define PWM_PIN_1 12
#define PWM_PIN_2 13

#define ADC_PIN 28
#define ADC_INPUT (ADC_PIN-26) //determined from Pico pinout diagram which GPIO# corresponds to which ADC# for adc_select_input.


int main()
{
    stdio_init_all();

//======turn on the adc [adapted from Pico example code]
    adc_init();
    adc_gpio_init(ADC_PIN); 
    adc_select_input(ADC_INPUT);

//======I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    uint ina219_config_reg = 0x00;
    uint ina219_config_value = 0b0011100011000111;
    uint8_t ina219_config_array[3];
    ina219_config_array[0]=ina219_config_reg;
    ina219_config_array[1]=ina219_config_value >>8;
    ina219_config_array[2]=ina219_config_value & 0b11111111;
    
    uint ina219_calib_reg = 0x05;
    uint ina219_calib_value = 1024;
    uint8_t ina219_calib_array[3];
    ina219_calib_array[0]=ina219_calib_reg;
    ina219_calib_array[1]=ina219_calib_value >>8;
    ina219_calib_array[2]=ina219_calib_value & 0b11111111;

    i2c_write_blocking(i2c_default, I2C_INA219_ADDR, ina219_config_array, 3, false);
    i2c_write_blocking(i2c_default, I2C_INA219_ADDR, ina219_calib_array, 3, false);

//======PWM init [adapted from Pico example code]
    gpio_set_function(PWM_PIN_1, GPIO_FUNC_PWM); // Set the Pin to be PWM
    gpio_set_function(PWM_PIN_2, GPIO_FUNC_PWM);

    uint pwm1_slice_num = pwm_gpio_to_slice_num(PWM_PIN_1); // Get PWM slice number
    uint pwm2_slice_num = pwm_gpio_to_slice_num(PWM_PIN_2);

    // the clock frequency is 150MHz divided by a float from 1 to 255
    float div = 10; // must be between 1-255
    pwm_set_clkdiv(pwm1_slice_num, div); // sets the clock speed
    pwm_set_clkdiv(pwm2_slice_num, div); // sets the clock speed

    uint16_t wrap = 3000; // when to rollover, must be less than 65535
    pwm_set_wrap(pwm1_slice_num, wrap); 
    pwm_set_wrap(pwm2_slice_num, wrap); 

    pwm_set_enabled(pwm1_slice_num, true); // turn on the PWM
    pwm_set_enabled(pwm2_slice_num, true);

    pwm_set_gpio_level(PWM_PIN_1, 1.*wrap); // set the duty cycle to 100% (stop motor)
    pwm_set_gpio_level(PWM_PIN_2, 1.*wrap);

//====== Init variables
    int totalDataPoints=100;
    int desiredCurrent;
    int desiredCurrentArray[totalDataPoints];
    int currentSensorInputArray[totalDataPoints];   
   
    char userInput;

    while (true) {
        sleep_ms(1000);
        printf("\n\n----------------------------------------------\n\n");
        printf("Start current sensor data collection? [Y/n]\n");
        scanf("%c", &userInput);
        printf("\n");

        if (userInput=='Y'){
            float errP=0.;
            float errI=0.;
            float kP=50.0;
            float kI=50.0;
                
            static int currentSensorMax=66000;
            static int delayTime=1;
  
            // I2C init for current sensor
            static char bufREAD[2];
            bufREAD[0]=0x09; //arbitrary data to send on read
            bufREAD[1]=0b00000001; //arbitrary data to send on read
            static char readDATA[2];
        
            for(int i=0; i<totalDataPoints; i++){
                //=== current sensor input
                
                //averaging filter: current reads
                int averageSampleNumber=3;
                int sumCurrentReads=0;
                for (int n=0; n<averageSampleNumber; n++){
                    i2c_write_blocking(i2c_default, I2C_INA219_ADDR, bufREAD, 2, true);  // true to keep host control of bus
                    i2c_read_blocking(i2c_default, I2C_INA219_ADDR, readDATA, 2, false);  // false - finished with bus
                    uint16_t currentSensorReadDATA=(readDATA[0]<<8) | readDATA[1];
                    //currentSensorInputArray[i]=currentSensorReadDATA;
                    sumCurrentReads=sumCurrentReads+currentSensorReadDATA;

                    //printf("%.8b %.8b :: %.5i\n", readDATA[0], readDATA[1], currentSensorReadDATA);
                    //printf("sum: %u\n", sumCurrentReads);
                    //GraphOnCLI(combinedReadDATA,CurrentSensorMax);
                }
                int avgCurrentRead=sumCurrentReads/averageSampleNumber;
                currentSensorInputArray[i]=avgCurrentRead;

                //=== PI control
                if(i<(totalDataPoints/4) || ((i>=(totalDataPoints)/2) && i<(3*totalDataPoints)/4)){
                    desiredCurrent=60000;
                    //printf("%i: DesiredCurrent=%i\n", i, desiredCurrent);
                }
                else if((i>=(totalDataPoints/4) && i<(totalDataPoints/2)) || (i>=(3*totalDataPoints)/4 && i<(totalDataPoints-2))){
                    desiredCurrent=6000;
                    //printf("%i: DesiredCurrent=%i\n", i, desiredCurrent);
                }
                else{
                    //printf("%i: MOTOR STOP\n", i);
                    pwm_set_gpio_level(PWM_PIN_1, wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, wrap);
                }
                desiredCurrentArray[i]=desiredCurrent;
                
                errP=desiredCurrentArray[i]-currentSensorInputArray[i];
                /*int errPmax=30000;
                if(errP>errPmax || errI<-errPmax){
                    errP=errPmax;
                }*/
                /*
                errI=errI+errP;
                int errImax=30000;
                if(errI>errImax || errI<-errImax){
                    errI=errImax;
                }*/

                float controlFactor= (kP*errP + kI*errI)/currentSensorMax;
                if(controlFactor>1){
                    controlFactor=1.0;
                }
                if(controlFactor<-1){
                    controlFactor=-1.0;
                }
                printf("%i: errP: %f, errI: %f, controlFactor %f",i,errP, errI, controlFactor);
                printf("readCurrent: %i,\n", currentSensorInputArray[i]);

                if (controlFactor>0){
                    pwm_set_gpio_level(PWM_PIN_1, wrap-(controlFactor*wrap)); 
                    pwm_set_gpio_level(PWM_PIN_2, wrap);
                }
                else if(controlFactor<0){
                    pwm_set_gpio_level(PWM_PIN_1, wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, wrap+(controlFactor*wrap));
                }
                
                //sleep_ms(delayTime);
            }

            //printf("MOTOR STOP\n");
            pwm_set_gpio_level(PWM_PIN_1, wrap); 
            pwm_set_gpio_level(PWM_PIN_2, wrap);

            //printf("\n\nDATA:\n");
            for(int i=0; i<totalDataPoints; i++){
                printf("%.3i %.5i %.5i CurrentSensorData\n", i, desiredCurrentArray[i], currentSensorInputArray[i]);
                //GraphOnCLI(currentSensorInputArray[i],66000);
                //GraphOnCLI((desiredCurrentArray[i]-currentSensorInputArray[i]),10000);
            }
            printf("[END]");
        }
        else{
            printf("\n--NO CURRENT DATA COLLECTED--\n"); 
        }

        //====== TEST - ADC input
        /*uint16_t ADCvalue = adc_read();
        printf("%.4u", ADCvalue);
        GraphOnCLI(ADCvalue,2047);
        */
        
        //====== TEST - current sensor
        /*char bufREAD[2];
        bufREAD[0]=0x09;
        bufREAD[1]=0b00000001;

        char readDATA[2];

        i2c_write_blocking(i2c_default, I2C_INA219_ADDR, bufREAD, 2, true);  // true to keep host control of bus
        i2c_read_blocking(i2c_default, I2C_INA219_ADDR, readDATA, 2, false);  // false - finished with bus
        
        int combinedReadDATA=readDATA[0]<<8 | readDATA[1];

        printf("%.8b %.8b :: %.4i", readDATA[0], readDATA[1], combinedReadDATA);
        GraphOnCLI(combinedReadDATA,6000);
       */

/*
    //====== TEST - PWM output to servo and ADC/current sensor input from servo
        static uint16_t ADCvalue;
        static int ADCmax=4095; 
        static int CurrentSensorMax=66000;
        static int delayTime=1;
        static float motorSpeedValue=0.8;
        static const int totalDataPoints=50;

        // I2C init for current sensor
        static char bufREAD[2];
        bufREAD[0]=0x09;
        bufREAD[1]=0b00000001;
        static char readDATA[2];

       
        for(int i=0; i<totalDataPoints; i++){
            switch (i)
                {
                case 0:
                    pwm_set_gpio_level(PWM_PIN_1, 1.0*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, 1.0*wrap);
                    break;
                case 1:
                    pwm_set_gpio_level(PWM_PIN_1, motorSpeedValue*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, 1.0*wrap);
                    break;
                case totalDataPoints/4:
                    pwm_set_gpio_level(PWM_PIN_1, 1.0*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, motorSpeedValue*wrap);
                    break;
                case (2*totalDataPoints)/4:
                    pwm_set_gpio_level(PWM_PIN_1, motorSpeedValue*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, 1.0*wrap);
                    break;
                case (3*totalDataPoints)/4:
                    pwm_set_gpio_level(PWM_PIN_1, 1.0*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, motorSpeedValue*wrap);
                    break;
                case (totalDataPoints-1):
                    pwm_set_gpio_level(PWM_PIN_1, 1.0*wrap); 
                    pwm_set_gpio_level(PWM_PIN_2, 1.0*wrap);
                    break;
                default:
                    //printf("<Switch mode ERROR>");
                    break;
                }  
            
            //=== current sensor input
            i2c_write_blocking(i2c_default, I2C_INA219_ADDR, bufREAD, 2, true);  // true to keep host control of bus
            i2c_read_blocking(i2c_default, I2C_INA219_ADDR, readDATA, 2, false);  // false - finished with bus
            int combinedReadDATA=readDATA[0]<<8 | readDATA[1];
            currentSensorInputArray[i]=combinedReadDATA;
            //printf("%.8b %.8b :: %.4i", readDATA[0], readDATA[1], combinedReadDATA);
            //GraphOnCLI(combinedReadDATA,CurrentSensorMax);
            sleep_ms(delayTime);
        }

        printf("\n\nDATA:\n");
        for(int i=0; i<totalDataPoints; i++){
            printf("%.5i",currentSensorInputArray[i]);
            GraphOnCLI(currentSensorInputArray[i],66000);
        }
        sleep_ms(10000);
*/
        
        //=== ADC input
        /*for(int i=0; i<=1000; i++){
            ADCvalue = adc_read();
            printf("%.4u", ADCvalue);
            GraphOnCLI(ADCvalue,ADCmax);
            sleep_ms(delayTime);
        }*/
        


    } //WHILE
} //MAIN
