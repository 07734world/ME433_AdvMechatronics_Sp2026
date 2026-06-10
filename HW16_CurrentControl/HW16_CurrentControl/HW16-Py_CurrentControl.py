# HW 16 - Python read current sensor data and plot
import serial
import numpy as np
import matplotlib.pyplot as plt

###########################################
#   Read data from serial port
#

ser = serial.Serial('COM3')#,115200,timeout=1)
print(f"Connected to {ser.name}")

userInput = input("Start current sensor data collection? [Y/n]")

ser.write(f"{userInput}\n".encode('utf-8'))
#ser.write(b'2\n')

sampleNumber=[]
desiredCurrent=[]
actualCurrent=[]
serialReadText=""
while True:
    serialRead=ser.readline()
    serialReadText=serialRead.decode('utf-8')
    #print(serialReadText)

    if 'CurrentSensorData' in serialReadText:
        sampleNumber.append(float(serialReadText[0:3]))
        desiredCurrent.append(int(serialReadText[4:9]))
        actualCurrent.append(int(serialReadText[10:15]))
        #print(f"sampleNumber:{sampleNumber}\ndesiredCurrent:{desiredCurrent}\nactualCurrent:{actualCurrent}\n")

    if '[END]' in serialReadText:
        print("Python: Serial Read Finished")
        break

ser.close()


###########################################
#   Plot data
#

fig, (ax1) = plt.subplots(1, 1)
fig.suptitle('Current Control', fontsize=8, fontweight='bold')
l1=ax1.plot(sampleNumber,desiredCurrent,'k-')
l2=ax1.plot(sampleNumber,actualCurrent,'r.')
ax1.set_xlabel('Time [arbitrary]')
ax1.set_ylabel('Current [arbitrary]')
fig.legend([l1, l2], labels=['Desired Current','Actual Current'],loc="lower right")
fig.tight_layout(pad=1.0)
filename='HW16-PNG_CurrentControl.png'
plt.savefig(str(filename))
plt.show()

print("<END SCRIPT>")
