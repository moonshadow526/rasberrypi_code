#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define BUF_SIZE 255
int sectionid = 0;
int i = 0;
/*
char def[]={0x1A};
char ent[1];
char rec[255]={0};
*/
struct data
{
    char gps_time[20];
    char gps_location;
    char gps_lat[12];
    char gps_long[12];
    char gps_date[8];
}GPS_Data;

void gps_resolve_GPRMC(char data)
{
//    int i = 0;

    if(data == ',')
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
		    GPS_Data.gps_location = '>';
		else
		    GPS_Data.gps_location = '<';
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
//    printf("date: %s\n", GPS_Data.gps_date);
    }

}

void read_gps_data(int fd)
{
    char buffer[BUF_SIZE],buff[BUF_SIZE],dest[255];
    char gps_id[8]="$GPRMC";
    int j = 0, k = 0;
    char *p;
    memset(dest, 0 , sizeof(dest));
    
//    while(1){
	memset(buffer, 0, sizeof(buffer));
	while(!serialDataAvail(fd));
        printf("%d\n", serialDataAvail(fd));
	for(j = 0; j < 40; j++){
	    read(fd, buffer, 10);
	    strcat(dest, buffer);
	}
        printf("s: %s\n", dest);
            printf("dest1:  %s\n", dest);
	    if((p = strstr(dest, gps_id)))
	    { 
                printf("*P : %x", p); 
		printf("dest2:  %s\n", dest);
		for(k = 0; k < strlen(dest); k++){
		    gps_resolve_GPRMC(*(p+k));
                    
		}
		printf("resolced....\n");
		printf("%s\n", GPS_Data.gps_lat);
		sectionid = 0;				
                printf("%s\n", GPS_Data.gps_long);

	    }	
	    bzero(dest, sizeof(dest));	
//	}	
//    }
}

int main(void)
{
    int fd, i = 0;
    char UartBuff[10]={0};
    ent[0] = 0x0a;
    if((fd = serialOpen("/dev/ttyAMA0", 9600)) < 0){
        printf("serial err\n");	
    }       

    while(!serialDataAvail(fd));
//    read(fd, rec, serialDataAvail(fd));
//	UartBuff[0] = serialGetchar(fd);
    read_gps_data(fd);
    return 0;

}



