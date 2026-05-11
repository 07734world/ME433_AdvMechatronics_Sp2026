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
    sampleRate=1/(t[1]-t[0]) # [Hz]

    return (t, data1, sampleRate)

ImportSigA=customCSVimport('sigA')
tA=ImportSigA[0]
dataA=ImportSigA[1]
print("A sample rate:"+str(ImportSigA[2])+"Hz")

ImportSigB=customCSVimport('sigB')
tB=ImportSigB[0]
dataB=ImportSigB[1]
print("B sample rate:"+str(ImportSigB[2])+"Hz")

ImportSigC=customCSVimport('sigC')
tC=ImportSigC[0]
dataC=ImportSigC[1]
print("C sample rate:"+str(ImportSigC[2])+"Hz")

ImportSigD=customCSVimport('sigD')
tD=ImportSigD[0]
dataD=ImportSigD[1]
print("D sample rate:"+str(ImportSigD[2])+"Hz")
#print(tD)
#print(dataD)
'''
while(1):
    waitForever=1
'''

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
def IIR(timeData, dataData, avgDataPtAmt, signalWeight):
    meanTimeArray=[]
    TimeSubarray=[]
    meanDataArray=[]
    DataSubarray=[]
    Bweight=signalWeight
    Aweight=1-signalWeight
    
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
        if((n+avgDataPtAmt+1) < len(timeData)):
            sigTime=timeData[n+avgDataPtAmt+1]
            sigData=dataData[n+avgDataPtAmt+1]
            IIRtime=Aweight*TimeSubarrayMean+Bweight*sigTime
            IIRdata=Aweight*DataSubarrayMean+Bweight*sigData

        TimeSubarray=[]
        DataSubarray=[]
        if((n+avgDataPtAmt) <= len(timeData)):
            meanTimeArray=np.append(meanTimeArray,IIRtime)
            meanDataArray=np.append(meanDataArray,IIRdata)
            #print("MeanTimeArray: ",meanTimeArray," meanDataArray: ",meanDataArray)
            print("MeanTime so Far:",meanTimeArray[n]," of ",timeData[-1])
        else:
            break

    return (meanTimeArray, meanDataArray, Aweight, Bweight)



#------ FINITE IMPULSE RESPONSE (FIR) ------
def FIR(timeData, dataData, coefficients):
    meanTimeArray=[]
    TimeSubarray=[]
    meanDataArray=[]
    DataSubarray=[]
       
    for n in range(len(timeData)):
        #print("n: ",n)
        for i in range(len(coefficients)):
            #print("i :",i)
            if((n+len(coefficients)) <= len(timeData)):
                TimeSubarray=np.append(TimeSubarray,timeData[int(n+i)])
                DataSubarray=np.append(DataSubarray,(dataData[int(n+i)]*coefficients[int(i)]))
                #print("TimeSubarry: ",TimeSubarray, " DataSubarry: ", DataSubarray)
                TimeSubarrayMean=np.mean(TimeSubarray)
                DataSubarrayMean=np.sum(DataSubarray)
                #print("TimeSubarrayMean: ",TimeSubarrayMean,"  DataSubarrayMean: ",DataSubarrayMean)
            else:
                #print("done subarray")
                break
        
        TimeSubarray=[]
        DataSubarray=[]
        if((n+len(coefficients)) <= len(timeData)):
            meanTimeArray=np.append(meanTimeArray,TimeSubarrayMean)
            meanDataArray=np.append(meanDataArray,DataSubarrayMean)
            #print("MeanTimeArray: ",meanTimeArray," meanDataArray: ",meanDataArray)
            print("MeanTime so Far:",meanTimeArray[n]," of ",timeData[-1])
        else:
            break

    return (meanTimeArray, meanDataArray)




#================= FAST FOURIER TRANSFORM (adapted from class example code) =================
def customFFTsingle(timeData, inputData,dataSet):
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
    title1='sig'+str(dataSet)+' FFT'
    title2='Unfiltered'
    fig.suptitle(str(title1)+'\n '+str(title2), fontsize=8, fontweight='bold')
    l1=ax1.plot(t,y,'k-')
    #l2=ax1.plot(tF,yF,'r-')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'k-') # plotting the fft
    #ax2.loglog(frqF,abs(YF),'r-') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    fig.legend([l1], labels=['Unfiltered'],loc="lower right")
    fig.tight_layout(pad=1.0)
    filename=str(title1)+str(title2)+'.png'
    plt.savefig(str(filename))
    plt.show()

    #return(t,y,frq,abs(Y))


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
    fig.suptitle(str(title1)+'\n '+str(title2), fontsize=8, fontweight='bold')
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
customFFTsingle(tA,dataA,'A')
customFFTsingle(tB,dataB,'B')
customFFTsingle(tC,dataC,'C')
customFFTsingle(tD,dataD,'D')


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
'''
Aavg=50 #50
sigA_IIR=IIR(tA,dataA,Aavg,0.1)
tA_IIR=sigA_IIR[0]
dataA_IIR=sigA_IIR[1]

Bavg=50 #50
sigB_IIR=IIR(tB,dataB,Bavg,0.1)
tB_IIR=sigB_IIR[0]
dataB_IIR=sigB_IIR[1]

Cavg=2 #2
sigC_IIR=IIR(tC,dataC,Cavg,0.1)
tC_IIR=sigC_IIR[0]
dataC_IIR=sigC_IIR[1]

Davg=10 #10
sigD_IIR=IIR(tD,dataD,Davg,0.1)
tD_IIR=sigD_IIR[0]
dataD_IIR=sigD_IIR[1]

customFFT(tA,dataA,tA_IIR,dataA_IIR,'A','Infinite Impulse Response Filter (Aavg='+str(sigA_IIR[2])+' Bsig='+str(sigA_IIR[3])+')')
customFFT(tB,dataB,tB_IIR,dataB_IIR,'B','Infinite Impulse Response Filter (Aavg='+str(sigB_IIR[2])+' Bsig='+str(sigB_IIR[3])+')')
customFFT(tC,dataC,tC_IIR,dataC_IIR,'C','Infinite Impulse Response Filter (Aavg='+str(sigC_IIR[2])+' Bsig='+str(sigC_IIR[3])+')')
customFFT(tD,dataD,tD_IIR,dataD_IIR,'D','Infinite Impulse Response Filter (Aavg='+str(sigD_IIR[2])+' Bsig='+str(sigD_IIR[3])+')')
'''


#==== FIR ====

#Low pass (windowed-sic FIR) from <https://fiiir.com/>
# sigA - sampleRate 10000 Hz
# sigB - sampleRate 3300 Hz
# sigC - sampleRate 2500 Hz
# sigD - sampleRate 400 Hz


#A_coeff: 47x weights, 100Hz cut-off, 1000Hz transition
A_coeff = [
    -0.000000000000000001,
    0.000064393676763220,
    0.000271319801257540,
    0.000649073204382278,
    0.001235623050647806,
    0.002076637553675906,
    0.003222058252059904,
    0.004721452177155942,
    0.006618513253826244,
    0.008945196904421814,
    0.011716041834802050,
    0.014923251981835048,
    0.018533075973444006,
    0.022483932798082779,
    0.026686597480116862,
    0.031026590809771163,
    0.035368727580917296,
    0.039563585562856544,
    0.043455480385182557,
    0.046891386356592825,
    0.049730143962839937,
    0.051851251307310706,
    0.053162553912562159,
    0.053606224358990776,
    0.053162553912562159,
    0.051851251307310706,
    0.049730143962839930,
    0.046891386356592839,
    0.043455480385182564,
    0.039563585562856544,
    0.035368727580917296,
    0.031026590809771187,
    0.026686597480116866,
    0.022483932798082775,
    0.018533075973443999,
    0.014923251981835051,
    0.011716041834802057,
    0.008945196904421819,
    0.006618513253826253,
    0.004721452177155945,
    0.003222058252059902,
    0.002076637553675908,
    0.001235623050647806,
    0.000649073204382280,
    0.000271319801257541,
    0.000064393676763220,
    -0.000000000000000001,
]
#B_coeff: 153x weights, 33Hz cut-off, 100Hz transition
B_coeff = [
    0.000000000000000000,
    -0.000000720860170916,
    -0.000002922026023300,
    -0.000006645442006556,
    -0.000011910067435400,
    -0.000018709228510322,
    -0.000027007399766794,
    -0.000036736428850418,
    -0.000047791239728572,
    -0.000060025070854593,
    -0.000073244325897368,
    -0.000087203134930557,
    -0.000101597742939378,
    -0.000116060859661942,
    -0.000130156119675172,
    -0.000143372813836725,
    -0.000155121062323613,
    -0.000164727605238994,
    -0.000171432388826330,
    -0.000174386123538883,
    -0.000172648984440316,
    -0.000165190614615539,
    -0.000150891578486668,
    -0.000128546394276369,
    -0.000096868253539958,
    -0.000054495510978232,
    0.000000000000000000,
    0.000068102800847834,
    0.000151341757935196,
    0.000251276270486603,
    0.000369480732828958,
    0.000507527382763517,
    0.000666967682969224,
    0.000849312416479448,
    0.001056010709785838,
    0.001288428227358375,
    0.001547824808701015,
    0.001835331842895296,
    0.002151929695376261,
    0.002498425516951251,
    0.002875431775397811,
    0.003283345855025347,
    0.003722331069105983,
    0.004192299423914947,
    0.004692896461208677,
    0.005223488488348065,
    0.005783152482084092,
    0.006370668923503328,
    0.006984517788117883,
    0.007622877877009221,
    0.008283629632814532,
    0.008964361538775322,
    0.009662380150716797,
    0.010374723761419955,
    0.011098179645160085,
    0.011829304778023334,
    0.012564449877808428,
    0.013299786556708930,
    0.014031337331383400,
    0.014755008189264406,
    0.015466623367804475,
    0.016161961965531206,
    0.016836795970945853,
    0.017486929268035419,
    0.018108237155980558,
    0.018696705905937267,
    0.019248471869855219,
    0.019759859655368021,
    0.020227418886936500,
    0.020647959086616246,
    0.021018582227914164,
    0.021336712542938167,
    0.021600123196066568,
    0.021806959476200694,
    0.021955758203751261,
    0.022045463097196568,
    0.022075435896610211,
    0.022045463097196568,
    0.021955758203751261,
    0.021806959476200694,
    0.021600123196066568,
    0.021336712542938167,
    0.021018582227914171,
    0.020647959086616246,
    0.020227418886936500,
    0.019759859655368021,
    0.019248471869855219,
    0.018696705905937274,
    0.018108237155980565,
    0.017486929268035415,
    0.016836795970945853,
    0.016161961965531210,
    0.015466623367804482,
    0.014755008189264413,
    0.014031337331383400,
    0.013299786556708935,
    0.012564449877808432,
    0.011829304778023334,
    0.011098179645160086,
    0.010374723761419954,
    0.009662380150716797,
    0.008964361538775329,
    0.008283629632814538,
    0.007622877877009223,
    0.006984517788117881,
    0.006370668923503327,
    0.005783152482084095,
    0.005223488488348069,
    0.004692896461208679,
    0.004192299423914948,
    0.003722331069105982,
    0.003283345855025347,
    0.002875431775397812,
    0.002498425516951254,
    0.002151929695376262,
    0.001835331842895295,
    0.001547824808701015,
    0.001288428227358376,
    0.001056010709785838,
    0.000849312416479448,
    0.000666967682969225,
    0.000507527382763517,
    0.000369480732828958,
    0.000251276270486603,
    0.000151341757935196,
    0.000068102800847834,
    0.000000000000000000,
    -0.000054495510978232,
    -0.000096868253539958,
    -0.000128546394276369,
    -0.000150891578486668,
    -0.000165190614615539,
    -0.000172648984440316,
    -0.000174386123538883,
    -0.000171432388826331,
    -0.000164727605238994,
    -0.000155121062323613,
    -0.000143372813836725,
    -0.000130156119675172,
    -0.000116060859661943,
    -0.000101597742939379,
    -0.000087203134930557,
    -0.000073244325897368,
    -0.000060025070854593,
    -0.000047791239728572,
    -0.000036736428850418,
    -0.000027007399766794,
    -0.000018709228510322,
    -0.000011910067435400,
    -0.000006645442006556,
    -0.000002922026023300,
    -0.000000720860170916,
    0.000000000000000000,
]
#C_coeff: 13x weights, 100Hz cut-off, 1000Hz transition
C_coeff = [
    -0.000000000000000002,
    0.004209993088108993,
    0.022505059716592359,
    0.063627763221890107,
    0.124457549151658770,
    0.182138357851727412,
    0.206122553940044723,
    0.182138357851727412,
    0.124457549151658839,
    0.063627763221890135,
    0.022505059716592359,
    0.004209993088109006,
    -0.000000000000000002,
]
#D_coeff: 37x weights, 20Hz cut-off, 50Hz transition
D_coeff = [
    0.000000000000000000,
    -0.000043852923645903,
    -0.000226035951650886,
    -0.000598188277924350,
    -0.001148964584708609,
    -0.001752834771183813,
    -0.002117144704474100,
    -0.001753228200866521,
    0.000000000000000001,
    0.003881401083077981,
    0.010544569791914539,
    0.020355466896088612,
    0.033202086419889164,
    0.048376440349371039,
    0.064579170812937708,
    0.080068884316422187,
    0.092939056557025487,
    0.101466828819572960,
    0.104452688736309376,
    0.101466828819572960,
    0.092939056557025487,
    0.080068884316422201,
    0.064579170812937722,
    0.048376440349371060,
    0.033202086419889178,
    0.020355466896088612,
    0.010544569791914539,
    0.003881401083077983,
    0.000000000000000001,
    -0.001753228200866520,
    -0.002117144704474105,
    -0.001752834771183814,
    -0.001148964584708610,
    -0.000598188277924350,
    -0.000226035951650886,
    -0.000043852923645903,
    0.000000000000000000,
]
'''
sigA_FIR=FIR(tA,dataA,A_coeff)
tA_FIR=sigA_FIR[0]
dataA_FIR=sigA_FIR[1]
customFFT(tA,dataA,tA_FIR,dataA_FIR,'A','Finite Impulse Response Filter (low-pass, 47x weights, 100Hz cut-off, 1000Hz transition)')

sigB_FIR=FIR(tB,dataB,B_coeff)
tB_FIR=sigB_FIR[0]
dataB_FIR=sigB_FIR[1]
customFFT(tB,dataB,tB_FIR,dataB_FIR,'B','Finite Impulse Response Filter (low-pass, 153x weights, 33Hz cut-off, 100Hz transition)')

sigC_FIR=FIR(tC,dataC,C_coeff)
tC_FIR=sigC_FIR[0]
dataC_FIR=sigC_FIR[1]
customFFT(tC,dataC,tC_FIR,dataC_FIR,'C','Finite Impulse Response Filter (low-pass, 13x weights, 100Hz cut-off, 1000Hz transition)')

sigD_FIR=FIR(tD,dataD,D_coeff)
tD_FIR=sigD_FIR[0]
dataD_FIR=sigD_FIR[1]
customFFT(tD,dataD,tD_FIR,dataD_FIR,'D','Finite Impulse Response Filter (low-pass 37x weights, 20Hz cut-off, 50Hz transition)')
'''