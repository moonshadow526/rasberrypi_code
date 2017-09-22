#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "pulse_sensor.h"
/*
#define true 1
#define false 0
*/
int BPM,Signal;
int IBI = 600;
unsigned char Pulse = false;
unsigned char Qs = false;
int rate[10];
unsigned int sampleCounter = 0;
unsigned int lastBeatTime = 0;
int P = 512;
int T = 512;
int thresh = 512;
int amp = 100;
int Num;
unsigned char firstBeat = true;
unsigned char secondBeat = false;

extern int clockspeed;
extern int spichannel;

/***********************************************************************************************
*函数名 ： pulse_signal_adc
*函数功能描述 ： pulse_sensor值ad转换
*函数参数 ： 无
*函数返回值 ： 转换后的ad值
***********************************************************************************************/

int pulse_signal_adc(void)
{

    unsigned char data[3];
    int rec;
	
    data[0] = 0x01;
    data[1] = 0x80;
    data[2] = 0x00;

    wiringPiSPIDataRW(spichannel, data, 3);    //写spi同时读取spi的值

    rec = ((1<<8)*(data[1]&0x02))+((1<<8)*(data[1]&0x01))+data[2];    //把三组数据融合到一起

    return rec;
}

/***********************************************************************************************
*函数名 ： period_elapsed
*函数功能描述 ： 心率计算
*函数参数 ： 无
*函数返回值 ： 无
***********************************************************************************************/

void period_elapsed(void)
{
//	printf("test..................................\n");
    int i = 0;
    unsigned int runningTotal;
    Signal = pulse_signal_adc();
	printf("signal: %d\n", Signal);
    sampleCounter = millis();                //时间计算
    Num = sampleCounter - lastBeatTime - send_msg_time;     
    if(Signal < thresh && Num > (IBI/5)*3){
        if(Signal < T){
            T = Signal;
        }
    }
    if(Signal > thresh && Signal > P){
        P = Signal;
    }

    if(Num > 250){
        if((Signal > thresh) && (Pulse == false) && (Num > (IBI/5)*3)){
            Pulse = true;
            IBI = sampleCounter - lastBeatTime;
            lastBeatTime = sampleCounter;

            if(secondBeat){
                secondBeat = false;
                for(i = 0; i <= 9; i++){
                    rate[i] = IBI;
                }
            }
            if(firstBeat){
	        firstBeat = false;
		secondBeat = true;
		return;
	    }
	    
            runningTotal = 0;
            for(i = 0; i <= 8; i++){
	        rate[i] = rate[i+1];
		runningTotal += rate[i];
	    }

            rate[9] = IBI;
            runningTotal += rate[9];
            runningTotal /= 10;
            BPM = 60000/runningTotal;
	    Qs = true;
        }
    }
    
    if(Signal < thresh && Pulse == true){
        Pulse = false;
	amp = P-T;
	thresh = amp/2 + T;
	P = thresh;
	T = thresh;
    }
    
    if(Num > 2500){
        thresh = 512;
        P = 512;
	T = 512;
	lastBeatTime = sampleCounter;
	firstBeat = true;
	secondBeat = false;
    }
}

