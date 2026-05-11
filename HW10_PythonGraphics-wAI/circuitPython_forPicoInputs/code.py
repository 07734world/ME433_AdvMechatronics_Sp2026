#print("Hello World!")

import board
import pwmio
import time

import analogio

# there are other libraries for controlling RC servo motors
# but really all you need is PWM at 50Hz
servo = pwmio.PWMOut(board.GP16, variable_frequency=True)
servo.frequency = 50 # hz

a0 = analogio.AnalogIn(board.A0) # read the voltage on A0
a1 = analogio.AnalogIn(board.A1) # read the voltage on A1

while True:
    # pulse 0.5 ms to 2.5 ms out of a possible 20 ms (50Hz)
    # for 0 degrees to 180 degrees
    # so duty_cycle can be 65535*0.5/20 to 65535*2.5/20
    # but check this, some servo brands might only want 1-2 ms
    
    print("("+str(a0.value)+","+str(a1.value)+")")
    time.sleep(1/40)
