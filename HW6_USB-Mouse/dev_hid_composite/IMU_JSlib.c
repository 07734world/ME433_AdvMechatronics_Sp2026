#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
//#include "heartbeat_v2.h"
//#include "OLED_JScode.h"
//#include "ssd1306.h"


// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5


//===============================================
//  IMU Chip
char ADDR = 0b1101000;

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

//Sleep time between register reads
#define BTW_REG_SLEEP 10

// H-L Combine function for data
int16_t HLcombine(char H, char L) {
    int16_t HLcombined=(H<<8)|L;
    return HLcombined;
}

//int XorY;
int16_t XYaccelRead(int XorY)
{
    //heartbeat();

    stdio_init_all();

    // I2C Initialisation. Using it at n Khz.
    i2c_init(I2C_PORT, 100*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // PowerOn IMU
    char powerOn[]={PWR_MGMT_1, 0x00};
    i2c_write_blocking(i2c_default, ADDR, powerOn, 2, false);
    
    //WhoAmI setup
    char whoAmI=WHO_AM_I;
    char whoAmIread;

    //Accel Config
    char accelConfig[]={ACCEL_CONFIG, 0b00000000};
    i2c_write_blocking(i2c_default, ADDR, accelConfig, 2, false);

    //Gyro Config
    char gyroConfig[]={GYRO_CONFIG, 0b00011000};
    i2c_write_blocking(i2c_default, ADDR, gyroConfig, 2, false);

    // Initialize readout variables
    char xReadAccelH, xReadAccelL, yReadAccelH, yReadAccelL, zReadAccelH, zReadAccelL;
    char tempReadH, tempReadL;
    char xReadGyroH, xReadGyroL, yReadGyroH, yReadGyroL, zReadGyroH, zReadGyroL;
    char xAccelH=ACCEL_XOUT_H;
    
    // Initialize OLED
    //ssd1306_setup();

    //while (true) {
        //heartbeat();
 
        // IMU WhoAmI Check: should = 0x68
        i2c_write_blocking(i2c_default, ADDR, &whoAmI, 1, true);  // true to keep host control of bus
        i2c_read_blocking(i2c_default, ADDR, &whoAmIread, 1, false);  // false - finished with bus
        //printf("WhoAmI=%x [=0x68]\n", whoAmIread);
        //printf("startPoint[=0x3B]=%x\n",xAccelH);

        //IMU Config Check
        /*printf("powerON=[%x,%x]\n"
            "accelConfig=[%x,%x]\n"
            "gyroConfig=[%x,%x]\n" 
            "\n", powerOn[0], powerOn[1], accelConfig[0], accelConfig[1], gyroConfig[0], gyroConfig[1]);
        */
        // TEST
        uint16_t test=HLcombine(0b11110000,0b11111111);
        printf("test(0x)=%x\n",test);

        uint8_t dataRead[14];
        // ALL readout

        i2c_write_blocking(i2c_default, ADDR, &xAccelH, 1, true);  // true to keep host control of bus
        i2c_read_blocking(i2c_default, ADDR, dataRead, 14, false);
        

        int16_t xAccel=HLcombine(dataRead[0],dataRead[1]);
        int16_t yAccel=HLcombine(dataRead[2],dataRead[3]);
        int16_t zAccel=HLcombine(dataRead[4],dataRead[5]);
        int16_t Temp=HLcombine(dataRead[6],dataRead[7]);
        int16_t xGyro=HLcombine(dataRead[8],dataRead[9]);
        int16_t yGyro=HLcombine(dataRead[10],dataRead[11]);
        int16_t zGyro=HLcombine(dataRead[12],dataRead[13]);
     

        Temp=((Temp/340.)+36.53);

        //int16_t TotalAccel[]={xAccel, yAccel};
        //return TotalAccel;
        //return xAccel;
        
        if(XorY==0){
            if((xAccel*xAccel)<500000){
                return 0;
            }
            else{
            return xAccel;
            }
        }
        if(XorY==1){
            if(yAccel*yAccel<500000){
                return 0;
            }
            else{
            return yAccel;
            }
        }
        
        
        /*
        //char xRead=(xReadH<<8)|xReadL;
        printf("Accel(x,y,z)=[%d,%d,%d]\n"
            "Gyro(x,y,z)=(%i,%i,%i)\n"
            "Temp=%i\n"
            "***\n", xAccel, yAccel, zAccel, xGyro, yGyro, zGyro, Temp);
        */
        
            /*printf("xReadAccelH=%x\n"
            "xReadAccelL=%x\n"
            "yReadAccelH=%x\n"
            "yReadAccelL=%x\n"
            "zReadAccelH=%x\n"
            "zReadAccelL=%x\n"
            ,xReadAccelH, xReadAccelL, yReadAccelH, yReadAccelL, zReadAccelH, zReadAccelL);*/
        
        //sprintf(message,"x_a=%i,y_a=%i",xAccel,yAccel);
        //drawMessage(1,1,message);
        
        /*
        // NOTE: OLED is 128 x 32 pixels
        int i;
        double sf=20000.0;

        // X accel plot
        if (xAccel*xAccel<250000){
            ssd1306_drawPixel(64,16,1);
        }
        else if (xAccel>500){
            for(i=0;i<((xAccel/sf)*64);i++){
                ssd1306_drawPixel(64-i,16,1);
            }
        }
        else if (xAccel<500){
            for(i=0;i<((-xAccel/sf)*64);i++){
                ssd1306_drawPixel(64+i,16,1);
        }
        }

        // Y accel plot
        if (yAccel*yAccel<250000){
            ssd1306_drawPixel(64,16,1);
        }
        else if (yAccel>500){
            for(i=0;i<((yAccel/sf)*16);i++){
                ssd1306_drawPixel(64,16+i,1);
            }
        }
        else if (yAccel<500){
            for(i=0;i<((-yAccel/sf)*16);i++){
                ssd1306_drawPixel(64,16-i,1);
        }
        }

        sprintf(message,"*");
        drawMessage(64-((xAccel/sf)*64),16+((yAccel/sf)*16),message);
        
        ssd1306_update();
        ssd1306_clear();
        */
    //}
}
