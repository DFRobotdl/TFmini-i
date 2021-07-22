#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>

#include <wiringSerial.h>

int recData(unsigned char* buf);
unsigned int CRC16_2(unsigned char *buf, int len);

int fd;

unsigned char Data[8] = {0};
int main()
{
    if ((fd = serialOpen("/dev/ttyUSB1", 115200)) < 0) {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return 0;
    }
    while(1){
        delay(100);
        printf("%d cm\n",recData(Data));
    }
    return 1;

}

int recData(unsigned char* buf)
{
    char ret = 0;
    int jl=0;
    long curr = millis();
    char ch = 0;
    unsigned char COM[8]={0x01, 0x03, 0x00, 0x00,0x00,0x01, 0x84, 0x0A};
    write(fd, COM, 8);
    while(!ret){
        if (millis() - curr > 1000){
            //write(fd, COM, 8);
            curr = millis();
            printf("OK\n");
        }

        if(serialDataAvail(fd) > 0){
            delay(10);
            if (read(fd, &ch, 1) == 1){
                if(ch == 0x01){
                    buf[0] = ch;
                    if (read(fd, &ch, 1) == 1){
                        if(ch == 0x03){
                            buf[1] = ch;
                            if (read(fd, &ch, 1) == 1){
                                if(ch == 0x02){
                                    buf[2] = ch;
                                    if (read(fd, &buf[3], 4) == 4){
                                    
                                        //for(int i=0; i<7; i++){
                                        // if(buf[i] < 0x10){
                                        //       printf("0");
                                        //    }
                                        //  printf("%x ", buf[i]);
                                        // }
                                         
                                        // printf("\n");
                                        // printf("%x\n", CRC16_2(buf, 5));
                                        // printf("%x\n", buf[5]*256+buf[6]);
                                        if(CRC16_2(buf, 5) == (buf[5] * 256 + buf[6])){
                                            jl = buf[3]*256+buf[4];
                                            ret = 1;
                                        }
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return jl;
}

unsigned int CRC16_2(unsigned char *buf, int len)
{
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (unsigned int)buf[pos];
    for (int i = 8; i != 0; i--)
    {
      if ((crc & 0x0001) != 0)
      {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else 
      {
        crc >>= 1; 
      }
    }
  }

  crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
  return crc;
}
