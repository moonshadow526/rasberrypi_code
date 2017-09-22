#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

//char s[6] = "hello";
int MPU6050_ADD= 0x70;
int recoder_imu_data(void)
{
        int ifd;
		FILE *fp;
        int ax,ay,az,tmp,gx,gy,gz;
        printf("This is the MPU6050 test!\n");
        wiringPiSetup();  
//        pinMode(8,I2C_PIN);
//       pinMode(9,I2C_PIN);
        if((ifd=wiringPiI2CSetup(MPU6050_ADD))<0){
                printf("can't find i2c!!\n");        
                return ifd;
        }
		
		if((fp = fopen("recoder_imu_data", "a+")) != NULL){
			printf("open success");
		}
		
        wiringPiI2CWriteReg8(ifd,0x6B,00);

        ax=wiringPiI2CReadReg8(ifd,0x3B)<<8|wiringPiI2CReadReg8(ifd,0x3C);
        ay=wiringPiI2CReadReg8(ifd,0x3D)<<8|wiringPiI2CReadReg8(ifd,0x3E);
        az=wiringPiI2CReadReg8(ifd,0x3F)<<8|wiringPiI2CReadReg8(ifd,0x40);
        gx=wiringPiI2CReadReg8(ifd,0x43)<<8|wiringPiI2CReadReg8(ifd,0x44);
        gy=wiringPiI2CReadReg8(ifd,0x45)<<8|wiringPiI2CReadReg8(ifd,0x46);
        gz=wiringPiI2CReadReg8(ifd,0x47)<<8|wiringPiI2CReadReg8(ifd,0x48);
        printf("ax=%d  ay=%d  az=%d\n",ax,ay,az);
        printf("gx=%d  gy=%d  gz=%d\n",gx,gy,gz);
        delay(100);
      
//	  fwrite(s, 5, 1, fp);
	  fprintf(fp,"ax=%d  ay=%d  az=%d\n",ax,ay,az );
	  fprintf(fp,"ax=%d  ay=%d  az=%d\n",gx,gy,gz );
	  close(ifd);
	  fclose(fp);
}
