CFLAGS = -lwiringPi -O0 -Wall -L /usr/lib -lwiringPiDev
#VPATH = add:sub
OBJS = imu.o gsm_gps.o pulse_sensor.o main.o
TARGET = proj
$(TARGET):$(OBJS)
		$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean:
		-$(RM) $(TARGET)
		-$(RM) $(OBJS)