import csv
import matplotlib.pyplot as plt
import numpy as np



#================= IMPORT DATA FROM CSV (adapted from class example code) =================

def customCSVimport(fileName):
    t = [] # column 0
    data1 = [] # column 1
    #data2 = [] # column 2

    with open(''+fileName+'.csv') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data1.append(float(row[1])) # second column
            #data2.append(float(row[2])) # third column

    '''
    for i in range(len(t)):
        # print the data to verify it was read
        print(str(t[i]) + ", " + str(data1[i])) # + ", " + str(data2[i]))
    '''

    return (t, data1)

ImportSigA=customCSVimport('sigA')
tA=ImportSigA[0]
dataA=ImportSigA[1]

ImportSigB=customCSVimport('sigB')
tB=ImportSigB[0]
dataB=ImportSigB[1]

ImportSigC=customCSVimport('sigC')
tC=ImportSigC[0]
dataC=ImportSigC[1]

ImportSigD=customCSVimport('sigD')
tD=ImportSigD[0]
dataD=ImportSigD[1]
#print(tD)
#print(dataD)



#================= FILTER DATA =================

#------ MOVING AVERAGE FILTER (MAF) ------
def MAF(avgDataPtAmt, timeData, dataData):
    meanTimeArray=[]
    TimeSubarray=[]
    meanDataArray=[]
    DataSubarray=[]

    for n in range(len(timeData)):
        #print("n: ",n)
        for i in range(avgDataPtAmt):
            #print("i :",i)
            if((n+avgDataPtAmt) <= len(timeData)):
                TimeSubarray=np.append(TimeSubarray,timeData[int(n+i)])
                DataSubarray=np.append(DataSubarray,dataData[int(n+i)])
                #print("TimeSubarry: ",TimeSubarray, " DataSubarry: ", DataSubarray)
                TimeSubarrayMean=np.mean(TimeSubarray)
                DataSubarrayMean=np.mean(DataSubarray)
                #print("TimeSubarrayMean: ",TimeSubarrayMean,"  DataSubarrayMean: ",DataSubarrayMean)
            else:
                #print("done subarray")
                break
        TimeSubarray=[]
        DataSubarray=[]
        if((n+avgDataPtAmt) <= len(timeData)):
            meanTimeArray=np.append(meanTimeArray,TimeSubarrayMean)
            meanDataArray=np.append(meanDataArray,DataSubarrayMean)
            #print("MeanTimeArray: ",meanTimeArray," meanDataArray: ",meanDataArray)
            print("MeanTime so Far:",meanTimeArray[n]," of ",timeData[-1])
        else:
            break

    return (meanTimeArray, meanDataArray)

  

#------ INFINITE IMPULSE RESPONSE (IIR) ------
def IIR():
    


#================= FAST FOURIER TRANSFORM (adapted from class example code) =================

def customFFT(timeData, inputData, timeDataFILTERED, inputDataFILTERED,dataSet,filterType):
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
    title1='sig'+str(dataSet)+' FFT Comparison'
    title2='Unfiltered v '+str(filterType)
    fig.suptitle(str(title1)+'\n '+str(title2))
    l1=ax1.plot(t,y,'k-')
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





#================= PLOT DATA  =================

'''
#==== MAF ====
Aavg=50 #50
sigA_MAF=MAF(Aavg,tA,dataA)
tA_MAF=sigA_MAF[0]
dataA_MAF=sigA_MAF[1]

Bavg=50 #50
sigB_MAF=MAF(Bavg,tB,dataB)
tB_MAF=sigB_MAF[0]
dataB_MAF=sigB_MAF[1]

Cavg=2 #2
sigC_MAF=MAF(Cavg,tC,dataC)
tC_MAF=sigC_MAF[0]
dataC_MAF=sigC_MAF[1]

Davg=10 #10
sigD_MAF=MAF(Davg,tD,dataD)
tD_MAF=sigD_MAF[0]
dataD_MAF=sigD_MAF[1]

customFFT(tA,dataA,tA_MAF,dataA_MAF,'A','Moving Average Filter ('+str(Aavg)+'x data point moving avg)')
customFFT(tB,dataB,tB_MAF,dataB_MAF,'B','Moving Average Filter ('+str(Bavg)+'x data point moving avg)')
customFFT(tC,dataC,tC_MAF,dataC_MAF,'C','Moving Average Filter ('+str(Cavg)+'x data point moving avg)')
customFFT(tD,dataD,tD_MAF,dataD_MAF,'D','Moving Average Filter ('+str(Davg)+'x data point moving avg)')
'''

#==== IIR ====
