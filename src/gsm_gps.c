#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "gsm_gps.h"

#define BUF_SIZE 4096
int sectionid = 0;
int i = 0;
int fd;

/***********************************************************************************************
*函数名 ： sendAT
*函数功能描述 ： 发送AT指令
*函数参数 ： GPRS命令  延时参数
*函数返回值 ： sim808硬件指令处理后的反馈
***********************************************************************************************/

static int sendAT(struct gprsdata *pgprs, char *out, unsigned int delay)
{

	int size = 0;
    int i = 0, j = 0;
    int ret = _ATOTIME;
    char uartbuff[40] = {0};
    char buff[40] ={0};    
	char clcbuff[10] = {0};

	printf("cfd.............:%d\n",fd);

    serialPuts(fd, pgprs->order);                //发送串口数据
    if((pgprs->type == _SENDEND))
    {
	ret = _ATOK;
        goto sim808_sendATRET;
    }

    while(!serialDataAvail(fd));
	printf("datanum: %d\n", serialDataAvail(fd));

	for(i = 0; i < 4; i++){
            read(fd, buff, 10);                   //读取串口缓冲区的值
	    strcat(uartbuff, buff);
        }
        printf("AT_rec_cmd: %s\n", uartbuff);
		
		//串口返回数据解析
        switch(pgprs->type)
        {
	    case _AT:
	    case _ATCGMR:
		if(strstr(uartbuff, "OK")){
		    ret = _ATOK;
		    goto sim808_sendATRET;
		}
		else
		{
		    ret = _ATERR;
		    goto sim808_sendATRET;
		}
	    case _ATCSCA:
		if(strstr(uartbuff, "OK")){
		    ret = _ATOK;
		    goto sim808_sendATRET;
		}
		else{
		    ret = _ATERR;
		    goto sim808_sendATRET;
		}
	    case _ATCMGF:
                if(strstr(uartbuff, "OK")){
                    ret = _ATOK;
                    goto sim808_sendATRET;
                }
                else{
                    ret = _ATERR;
                    goto sim808_sendATRET;
				}
            case _ATCMGS:
                if(strstr(uartbuff, "AT+CMGS")){
                    ret = _ATOK;
                    goto sim808_sendATRET;
                }
                else{
                    ret = _ATERR;
                    goto sim808_sendATRET;
				}
            case _SENDENDEND:
                ret = _ATOK;
                goto sim808_sendATRET;
			case _ATCGPSPWR:
				if(strstr(uartbuff, "OK")){
					ret = _ATOK;
					goto sim808_sendATRET;
				}
				else
				{
					ret = _ATERR;
					goto sim808_sendATRET;
				}
			case _ATCGPSRST:
				if(strstr(uartbuff, "OK")){
					ret = _ATOK;
					goto sim808_sendATRET;
				}
				else
				{
					ret = _ATERR;
					goto sim808_sendATRET;
				}			
			case _ATCGPSOUTO:
				if(strstr(uartbuff, "OK")){
					ret = _ATOK;
					goto sim808_sendATRET;
				}
				else
				{
					ret = _ATERR;
					goto sim808_sendATRET;
				}	
			case _ATCGPSOUTC:
				if(strstr(uartbuff, "OK")){
					ret = _ATOK;
					goto sim808_sendATRET;
				}
				else
				{
					ret = _ATERR;
					goto sim808_sendATRET;
				}					
	    }
        
    sim808_sendATRET:
    memset(uartbuff, 0, sizeof(char)*40);
	memset(buff, 0, sizeof(char)*40);

    return ret;
}

/***********************************************************************************************
*函数名 ： sim808_exeorder
*函数功能描述 ： 执行AT命令
*函数参数 ： GPRS命令  命令类型  发送次数
*函数返回值 ： 发送后的成功或者失败反馈
***********************************************************************************************/

int sim808_exeorder(char *porder, enum order type, int num)
{
    int i = 0;
    int ret = _ATOTIME;
    int delay_time = 50;
    struct gprsdata gprs;

    gprs.order = porder;
    gprs.type = type;
    while((ret = sendAT(&gprs, NULL, delay_time)) != _ATOK)
    {
        if(ret == _ATERR){
            if(++i >= num)
            return ret;
        }
        else
        return _ATOTIME;
    }
    return _ATOK;
}

/***********************************************************************************************
*函数名 ： sim808_msg_send
*函数功能描述 ： 短信发送的处理流程
*函数参数 ： 短信内容
*函数返回值 ： 发送后的成功或者失败反馈
***********************************************************************************************/

int sim808_msg_send(char *pmsg)
{
    char end[] = {0x1A};

    int ret = _ATOTIME;

    if(_ATOK != (ret = sim808_exeorder(ATCSCA, _ATCSCA, 2)))
        goto sim808_msg_sendend;
    if(_ATOK != (ret = sim808_exeorder(ATCMGF, _ATCMGF, 2)))
        goto sim808_msg_sendend;
    if(_ATOK != (ret = sim808_exeorder(ATCMGS, _ATCMGS, 0)))
        goto sim808_msg_sendend;
    if(_ATOK == (ret = sim808_exeorder(pmsg, _SENDEND,0))){
	ret = sim808_exeorder(end, _SENDENDEND, 0);
    }
    sim808_msg_sendend:
    return ret;
}

/***********************************************************************************************
*函数名 ： sim808_gps_init
*函数功能描述 ： gps初始化函数
*函数参数 ： 无
*函数返回值 ： 初始化后的成功或者失败反馈
***********************************************************************************************/

int sim808_gps_init(void)
{
	int ret = _ATOTIME;
	if(_ATOK != (ret = sim808_exeorder(ATCGPSPWR, _ATCGPSPWR, 2)))
        goto sim808_gps_init_initend;
    if(_ATOK != (ret = sim808_exeorder(ATCGPSRST, _ATCGPSRST, 2)))
        goto sim808_gps_init_initend;
    sim808_gps_init_initend:
    return ret;
	printf("%s\n", ret);
}

/***********************************************************************************************
*函数名 ： gps_resolve_GPRMC
*函数功能描述 ： gps数据解析
*函数参数 ： gps数据包
*函数返回值 ： 无
***********************************************************************************************/

void gps_resolve_GPRMC(char data)
{

    if(data == ',')                        //以，为分隔符
    {
	++sectionid;
	i = 0;
    } 
    else
    {
	switch(sectionid)
	{
	    case 1:
		GPS_Data.gps_time[i++] = data;
		if(i == 2 || i == 5)
		    GPS_Data.gps_time[i++]=':';
		GPS_Data.gps_time[8] = '\0';
                printf("%s\n", GPS_Data.gps_time);
		break;
	    case 2:
		if(data == 'A')
		    GPS_Data.gps_location = 'T';
		else
		    GPS_Data.gps_location = 'F';
		break;
	    case 3:
		GPS_Data.gps_lat[++i] = data;
		GPS_Data.gps_lat[12] = '\0';
		break;
	    case 4:
		if(data == 'N')
		    GPS_Data.gps_lat[0] = 'N';
		else if(data == 'S')
		    GPS_Data.gps_lat[0] = 'S';
		break;
	    case 5:
		GPS_Data.gps_long[++i] = data;
		GPS_Data.gps_long[12] = '\0';
	        break;
	    case 6:
		if(data == 'E')
		    GPS_Data.gps_long[0] = 'E';
		else if(data == 'W')
		    GPS_Data.gps_long[0] = 'W';
   	        break;
	    case 7: break;
	    case 8: break;
	    case 9: 
		GPS_Data.gps_date[i++] = data;
		if(i == 2 || i== 5)
		    GPS_Data.gps_date[i++] = '-';
		GPS_Data.gps_date[8] = '\0';
		printf("date: %s\n", GPS_Data.gps_date);
		break;	
	}
//    printf("date: %s\n", GPS_Data.gps_lat);
    }

}

/***********************************************************************************************
*函数名 ： read_gps_data
*函数功能描述 ： 读取gps数据
*函数参数 ： fd描述符
*函数返回值 ： 无
***********************************************************************************************/

int read_gps_data(int fd)
{

    char buffer[BUF_SIZE],dest[BUF_SIZE];
    char gps_id[8]="$GPRMC";
    int j = 0, k = 0;
    char *p;
	printf("1\n");

	printf("gpsfd.............:%d\n",fd);

    memset(dest, 0 , sizeof(char)*BUF_SIZE);
    printf("2\n");
	memset(buffer, 0, sizeof(char)*BUF_SIZE);
	printf("3\n");
	while(!serialDataAvail(fd));
        printf("gps_date: %d\n", serialDataAvail(fd));
		sim808_exeorder(ATCGPSOUTC, _ATCGPSOUTC, 2);
	for(j = 0; j < 48; j++){
	    read(fd, buffer, 10);
	    strcat(dest, buffer);
	}
        printf("dest0: %s\n", dest);
            printf("dest1:  %s\n", dest);
	    if((p = strstr(dest, gps_id)))
	    { 
			printf("dest2:  %s\n", dest);
			for(k = 0; k < sizeof(dest)/sizeof(char); k++){
				gps_resolve_GPRMC(*(p+k));
                    
			}

			printf("resolved....\n");
		printf("%s\n", GPS_Data.gps_long);
			sectionid = 0;				

			if(GPS_Data.gps_location == 'T'){
				for(j = 0; j < sizeof(GPS_Data.gps_lat)/sizeof(char); j++){
					GPS_Data.gps_lastlat[j] = GPS_Data.gps_lat[j];
					GPS_Data.gps_lastlong[j] = GPS_Data.gps_long[j];
				}	
			}
			printf("last_long %s\n", GPS_Data.gps_lastlong);
	    }	
	    bzero(dest, sizeof(char)*BUF_SIZE);	
		printf("exit\n");
     	serialFlush(fd);
	    return 0;
}

/***********************************************************************************************
*函数名 ： init_gps
*函数功能描述 ： gps初始化
*函数参数 ： 无
*函数返回值 ： 0或者-1
***********************************************************************************************/

int init_gps(void)
{
	if(_ATOK == sim808_gps_init())
		return 0;
	else
		return -1;
}





