#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>
#include <lcd.h>
#include "pulse_sensor.h"
#include "gsm_gps.h"

int fspi;
int clockspeed = 500000;
int spichannel = 0;
int args[3] = {0};
int send_msg_time;

/***********************************************************************************************
*函数名 ： main
*函数功能描述 ： 主循环
*函数参数 ： 最小心率限制参数  最大心率限制参数
*函数返回值 ： 无
***********************************************************************************************/

int main (int argc, char *argv[])
{
	int i = 0;
	int bpm = 60;
	int ret = _ATOTIME;
	int bkey=1, skey=1;
	char bflag = 0;
	char sflag = 1;
	char iic_flag = 1;
	char send_buff[50] = {0};
	
	wiringPiSetup();
		
	
	//SPI初始化
    fspi = wiringPiSPISetup(spichannel, clockspeed);
    if(fspi < 0){
        printf("error setup wiringpispi\n");
        return -1;
    }
	//串口初始化
	if((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0){
        printf("serial err\n");
		return -1;
    }
	//输入参数检测 ./proj 最小心率限制参数 最大心率限制参数
	if(argc != 3){
		printf("args should be 3: ./proj minBQ maxBQ\n");
		return -1;
	}   

	int lfd = lcdInit (2, 16, 8,  21,22 , 0,1,2,3,4,5,6,7) ; //行，列，位，RS，En，数据脚8个   RW直接接地拉低
    if (lfd == -1)
    {
        printf ("lcdInit 1 failed\n") ;
        return 1 ;
    }
    sleep (1) ; //显示屏初始化
	
	//按键初始化 
	pinMode(23, INPUT);
	pullUpDnControl(23, PUD_UP);
	pinMode(24, INPUT);
	pullUpDnControl(24, PUD_UP);
	pinMode(25, INPUT);
	pullUpDnControl(25, PUD_UP);
	
	args[1] = atoi(argv[1]);      //输入参数转int型
	args[2] = atoi(argv[2]);
	
	while(init_gps() < 0){
		printf("init_gps err\n");
		init_gps();
	}      //gps定位初始化
	
	void overturn(void)       //中断处理函数
	{
		sflag = !sflag;
	}
	
	void iic_overturn(void)
	{
		iic_flag = !iic_flag;
	}
	
	wiringPiISR (23, INT_EDGE_RISING, iic_overturn);
	wiringPiISR (25, INT_EDGE_RISING, overturn);     //检测25脚上的上升沿进入中断
	
    while(1){
		bkey = digitalRead(24);
		skey = digitalRead(25);

		//主动发送定位
		if(bkey == 0){
			while(bkey == 0){
				bkey = digitalRead(24);
			}
			bflag = 1;
		}
		
		//关闭/打开心率检测
		if(sflag == 1){
	//		printf("...........................................masure\n");
			period_elapsed();
			send_msg_time = 0;
		}	
		
		//检测到心率信号
		if(Qs == true){
			if(BPM > 200 || BPM <45)      //判断心率是否有效
				continue;
			if(BPM  <　50){
				for(i = 0; i < 10; i++){
					period_elapsed();
				}
			}	
			printf("B: %d\n", BPM);
	//		printf("Q: %d\n", IBI);
			bpm = BPM;
			Qs = false;
		}
//		printf("here..................................\n");
		//满足条件发送定位
		if(bpm > args[2] || bpm < args[1] || bflag == 1){
			if(_ATOK == (ret = sim808_exeorder(ATCGPSOUTO, _ATCGPSOUTO, 0))){        //接收定位数据
			    send_msg_time = millis();
				read_gps_data(fd);
				sim808_exeorder(ATCGPSOUTC, _ATCGPSOUTC, 2);        //关闭定位
				printf("%s\n", GPS_Data.gps_lat);
				printf("%s\n", GPS_Data.gps_long);	
				
				//正确定位发送实时定位数据
				if(GPS_Data.gps_location == 'T' && GPS_Data.gps_lat[0] != NULL &&  GPS_Data.gps_long[0] != NULL){
					strcat(send_buff, "realtime location\n");
					strcat(send_buff, GPS_Data.gps_lat);
					strcat(send_buff, "\n");
					strcat(send_buff, GPS_Data.gps_long);
				}
				
				//未正确定位发送上次定位数据
				else if(GPS_Data.gps_location == 'F' && GPS_Data.gps_lat[0] != NULL &&  GPS_Data.gps_long[0] != NULL){
					strcat(send_buff, "lasttime location\n");
					strcat(send_buff, GPS_Data.gps_lastlat);
					strcat(send_buff, "\n");
					strcat(send_buff, GPS_Data.gps_lastlong);
				}
				
				//一直未正确定位发送定位错误
				else{
					memset(send_buff, 0, sizeof(char)*32);
					strcpy(send_buff, "error location");
				}
				
				printf("send msg\n.............");
				
				//发送到指定手机
				if(_ATOK != sim808_msg_send(send_buff)){
					printf("send err....");
					bflag = 0;
					delay(20);
				}				
//				i = 0;
				if(iic_flag == 1)
				recoder_imu_data();
			}
			printf("send_buff %s\n", send_buff);
			memset(send_buff, 0, sizeof(char)*32);        //清空buff
			bflag = 0;
			bpm = 60;
	//		delay(50);
			send_msg_time = millis() -send_msg_time;
		}			
    }
}