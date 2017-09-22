#ifndef __GSM_GPS_H__
#define __GSM_GPS_H__

#define AT         "AT\r\n"
#define ATCGMR     "AT+CGMR\r\n"
#define ATCSCA     "AT+CSCA?\r\n"
#define ATCMGF     "AT+CMGF=1\r\n"
#define ATCMGS     "AT+CMGS=\"18874069074\"\r\n"
#define SENDEND     0x1A
#define ATCGPSPWR   "AT+CGPSPWR=1\r\n"
#define ATCGPSRST   "AT+CGPSRST=1\r\n"
#define ATCGPSOUTO  "AT+CGPSOUT=255\r\n"           //gps接收数据开
#define ATCGPSOUTC  "AT+CGPSOUT=0\r\n"             //gps接收数据关

#define _ATOK 0
#define _ATERR -1
#define _ATOTIME -2

extern int fd;

enum order{
    _AT = 1, _ATCGMR, _ATCSCA, _ATCMGF, _ATCMGS, _SENDEND,
     _SENDENDEND, _ATCGPSPWR, _ATCGPSRST, _ATCGPSOUTO, _ATCGPSOUTC
};

struct gprsdata{
    char *order;
    int olen;
    enum order type;
};

struct data
{
    char gps_time[20];
    char gps_location;
    char gps_lat[14];
	char gps_lastlat[14];
    char gps_long[14];
	char gps_lastlong[14];
    char gps_date[10];
}GPS_Data;


int sim808_exeorder(char *porder, enum order type, int num);
int sim808_msg_send(char *pmsg);
int read_gps_data(int fd);
int init_gps(void);

#endif

