# HW 18 - Python plots of predicted desired force as a function of displacement

import serial
import numpy as np
import matplotlib.pyplot as plt

###########################################
#   SETUP FUNCTIONS
#

dataPoints=100

### DISPLACEMENT
#       equivalent to X-axis movement
#
displacement=[]
for i in range(1, dataPoints+1):
    displacement.append(i)

print(displacement)

### TOPOLOGY
#       equivalent to Y-axis movement
#
height=[]
Qsize=int(dataPoints/4)


# Q1
for i in range(1,Qsize+1):
    height.append(0)

# Q2
y=0
for i in range(1,Qsize+1):
    #y=i # straight line up
    y=height[-1]+(Qsize-(i-1))/Qsize
    if i > (1/3)*Qsize and i < (2/3)*Qsize: # do something in second 1/3 of Q2
        y=height[-1]-((i-1)/(0.9*Qsize))
    height.append(y)

# Q3
for i in range(1,Qsize+1):
    #y=(dataPoints/4)-i # straight line down
    y=height[-1]-((i-1)/Qsize)
    if i > (1/3)*Qsize and i < (2/3)*Qsize: # do something in second 1/3 of Q2
        y=height[-1]+((i-1)/(0.9*Qsize))
    height.append(y)

# Q4
for i in range(1,Qsize+1):
    height.append(0)



### FORCE
#       using simple spring/Hooke's law F=kx as a starting point
#           here k=arbitrary, and x=change in height

force=[]
F=0
k=12
for i in range(0,len(height)):
    F=k*(height[i]-height[i-1])
    force.append(F)


###########################################
#   PLOT
#
print(height)
plt.plot(displacement, height, 'k.', displacement, force, 'rx')
plt.axis((0,100,-15,15))
plt.title('"Cat Ear" Topology and Resulting Force')
plt.xlabel('Displacement [arbitrary units]')
plt.ylabel('Height or Force [arbitrary units]')
plt.legend(['Height','Force'])
plt.show()


'''
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
'''
