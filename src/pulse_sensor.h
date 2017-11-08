#ifndef __PULSE_SENSOR_H__
#define __PULSE_SENSOR_H__

#define true 1
#define false 0

extern int fspi;
//extern int BPM,Signal;

extern int IBI;
extern int BPM;
extern unsigned char Qs;
extern int send_msg_time;

void period_elapsed(void);

#endif

