# HW 14 - Python read force sensor data and plot
import serial
import numpy as np
import matplotlib.pyplot as plt

###########################################
#   Read data from serial port
#

ser = serial.Serial('COM3')#,115200,timeout=1)
print(f"Connected to {ser.name}")

userInput = input("User input to serial connection (number of seconds of data):")

ser.write(f"{userInput}\n".encode('utf-8'))
#ser.write(b'2\n')

time=[]
dataRAW=[]
dataIIR=[]
serialReadText=""
while True:
    serialRead=ser.readline()
    serialReadText=serialRead.decode('utf-8')
    #print(serialReadText)

    if 'ForceSensorData:' in serialReadText:
        time.append(float(serialReadText[16:24]))
        dataRAW.append(int(serialReadText[25:35]))
        dataIIR.append(int(serialReadText[36:46]))
        #print(f"time:{time}\ndataRAW:{dataRAW}\ndataIIR:{dataIIR}\n")

    if '[END]' in serialReadText:
        startTime=time[0]
        for n in range(len(time)):
            time[n]=(time[n]-startTime)/1000. #convert msec -> sec, normalize start time to zero
            #print(f"time:{time[n]}\ndataRAW:{dataRAW[n]}\ndataIIR:{dataIIR[n]}\n")
        print("Python: Serial Read Finished")
        break

ser.close()


###########################################
#   Plot data
#

#=== FFT function adapted from HW9
def customFFT(timeData, inputData, timeDataFILTERED, inputDataFILTERED):
    '''
    dt = 1.0/10000.0 # 10kHz
    t = np.arange(0.0, 1.0, dt) # 10s
    # a constant plus 100Hz and 1000Hz
    s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25
    '''
    s=inputData
    t=timeData
    Fs=len(timeData)/(timeData[-1]-timeData[0])
    #Fs = 10000 # sample rate
    Ts=(timeData[-1]-timeData[0])

    Ts = 1.0/Fs; # sampling interval
    ts = np.arange(0,t[-1],Ts) # time vector
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    sF=inputDataFILTERED
    tF=timeDataFILTERED
    FsF=len(timeDataFILTERED)/(timeDataFILTERED[-1]-timeDataFILTERED[0])
    #Fs = 10000 # sample rate
    TsF=(timeDataFILTERED[-1]-timeDataFILTERED[0])

    TsF = 1.0/FsF; # sampling interval
    tsF = np.arange(0,tF[-1],TsF) # time vector
    yF = sF # the data to make the fft from
    nF = len(yF) # length of the signal
    kF = np.arange(nF)
    TF = nF/FsF
    frqF = kF/TF # two sides frequency range
    frqF = frqF[range(int(nF/2))] # one side frequency range
    YF = np.fft.fft(yF)/nF # fft computing and normalization
    YF = YF[range(int(nF/2))]

    fig, (ax1, ax2) = plt.subplots(2, 1)
    title1='Force Sensor Data & FFT'
    title2='Unfiltered & IIR filter'
    fig.suptitle(str(title1)+'\n '+str(title2), fontsize=8, fontweight='bold')
    l1=ax1.plot(t,y,'k.')
    l2=ax1.plot(tF,yF,'r-')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k-') # plotting the fft
    ax2.loglog(frqF,abs(YF),'r-') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    fig.legend([l1, l2], labels=['Unfiltered','Filtered'],loc="lower right")
    fig.tight_layout(pad=1.0)
    filename=str(title1)+str(title2)+'.png'
    plt.savefig(str(filename))
    plt.show()

    #return(t,y,frq,abs(Y))

def customFFTsingle(timeData, inputData):
    '''
    dt = 1.0/10000.0 # 10kHz
    t = np.arange(0.0, 1.0, dt) # 10s
    # a constant plus 100Hz and 1000Hz
    s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25
    '''
    s=inputData
    t=timeData
    Fs=len(timeData)/(timeData[-1]-timeData[0])
    #Fs = 10000 # sample rate
    Ts=(timeData[-1]-timeData[0])

    Ts = 1.0/Fs; # sampling interval
    ts = np.arange(0,t[-1],Ts) # time vector
    y = s # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]


    fig, (ax1, ax2) = plt.subplots(2, 1)
    title1='Force Sensor Data & FFT'
    #title2='Unfiltered'
    fig.suptitle(str(title1)+'\n '+str(title2), fontsize=8, fontweight='bold')
    l1=ax1.plot(t,y,'k-')
    #l2=ax1.plot(tF,yF,'r-')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k-') # plotting the fft
    #ax2.loglog(frqF,abs(YF),'r-') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    #fig.legend([l1], labels=['Unfiltered'],loc="lower right")
    fig.tight_layout(pad=1.0)
    filename=str(title1)+str(title2)+'.png'
    plt.savefig(str(filename))
    plt.show()

    #return(t,y,frq,abs(Y))




#==== PLOT DATA

customFFT(time,dataRAW,time,dataIIR)
#customFFTsingle(time,dataIIR)

print("<END SCRIPT>")
