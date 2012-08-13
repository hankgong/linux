import time
import os
import subprocess
import datetime

saveValue = ""
pid = 0
indexPlaying = 0
popen = None
isProcFinished = True
savedTime = datetime.datetime.now()

def I2CGet(opts):
    return os.popen("i2cget -y " + opts).readline().strip()

def I2CSet(opts):
    os.system("i2cset -y " + opts)

def HDMIDiscovery():
    if(I2CGet("1 0x60 0xE0") != "0x80"):
        print "------------HDMIDiscovery------------"
        I2CSet("1 0x60 0xE0 0x01")
        while(I2CGet("1 0x60 0xE0")!="0x80"):
            time.sleep(1)
        print "_________HDMIDiscovery DONE__________"
    
def SendOutCECCode(opts):
    A6Reg = I2CGet("1 0x60 0xA6")
    A7Reg = I2CGet("1 0x60 0xA7")
    #while((A6Reg != "0x37" and A6Reg != "0xb7" and A6Reg !="0x97" and A6Reg !="0x87") or (A7Reg != "0x09" and A7Reg != "0x08" and A7Reg != "0x00")):
    while((A6Reg == "0xf7" or A6Reg == "0x77") or (A7Reg != "0x09" and A7Reg != "0x08" and A7Reg != "0x00")):
        time.sleep(0.05)
        A6Reg = I2CGet("1 0x60 0xA6")
        A7Reg = I2CGet("1 0x60 0xA7")
        print "A6Reg", A6Reg, "A7Reg", A7Reg
    	
    print I2CGet("1 0x60 0xA6"), I2CGet("1 0x60 0xA7")
    I2CSet("1 0x60 0x9F "+ opts)

def SwitchHDMI(port):
    print "------------SWITCHING TO HDMI" + str(port) +"------------"
    HDMIDiscovery()
    os.system("i2cset -y 1 0x60 0x88 0x0E")
    os.system("i2cset -y 1 0x60 0x89 0x0F")
    os.system("i2cset -y 1 0x60 0x8F 0x82")
    os.system("i2cset -y 1 0x60 0x90 0x" + str(port) + "0")
    os.system("i2cset -y 1 0x60 0x91 0x00")
    #os.system("i2cset -y 1 0x60 0x9F 0x12")
    print "Sending Active source"
    SendOutCECCode("0x12")
    os.system("i2cset -y 1 0x60 0x8F 0x04")
    print "Sending Image view on"
    #os.system("i2cset -y 1 0x60 0x9F 0x20")
    SendOutCECCode("0x20")


def PlayVideo(fileName):
    print "------------PLAYVIDEO------------"
    global pid
    global popen
    global pStatus
    global isProcFinished

    if(isProcFinished == False):
        os.system("kill -9 "+str(pid))

    SwitchHDMI(2)
    pStatus = None
    popen = subprocess.Popen(["gplay", "/media/"+fileName ])
    isProcFinished = False
    pid = popen.pid


def CheckButton():
    global indexPlaying
    global isProcFinished
    global savedTime

    getValue = I2CGet("2 0x55")
    curTime = datetime.datetime.now()
    difTime = curTime - savedTime

    if(getValue != "0x00" and difTime.seconds>5):
        if(getValue == "0x01" and indexPlaying !=1):
            indexPlaying = 1
            print "playing 1"
            PlayVideo("1.ts")
            savedTime = curTime
        elif(getValue == "0x02" and indexPlaying !=2):
            indexPlaying = 2
            print "playing 2"
            PlayVideo("2.ts")
            savedTime = curTime
        elif(getValue == "0x04" and indexPlaying !=3):
       	    indexPlaying = 3
       	    print "playing 3"
       	    PlayVideo("3.ts")
       	    savedTime = curTime
        elif(getValue=="0x08" and indexPlaying !=4):
            SwitchHDMI(2)
            indexPlaying = 4
            savedTime = curTime
        elif(getValue=="0x10" and indexPlaying !=5):
            SwitchHDMI(1)
            indexPlaying = 5
            savedTime = curTime

        saveValue = getValue
        #savedTime = curTime



if __name__ == "__main__":
    I2CSet("2 0x55 0xFF")
    while(1):
        CheckButton()
        if(popen!=None and isProcFinished==False):
            pStatus = popen.poll()

            if(pStatus != None):
                SwitchHDMI(1)
                popen = None
