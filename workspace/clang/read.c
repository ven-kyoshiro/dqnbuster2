/*
* Read MCP3002/MCP3204 using ioctl
*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

int spi_cs_fd;
unsigned char spi_mode;
unsigned char spi_bitsPerWord;
unsigned int spi_speed;

#define DEBUG 1


int SpiOpenPort (char *device)
{
    int status_value = -1;

    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0)     CPOL=0 (Clock Idle low level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_1 (0,1)     CPOL=0 (Clock Idle low level), CPHA=1 (SDO transmit/change edge idle to active)
    //SPI_MODE_2 (1,0)     CPOL=1 (Clock Idle high level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_3 (1,1)     CPOL=1 (Clock Idle high level), CPHA=1 (SDO transmit/change edge idle to active)
    spi_mode = SPI_MODE_0;
   
    //----- SET BITS PER WORD -----
    spi_bitsPerWord = 8;
   
    //----- SET SPI BUS SPEED -----
    spi_speed = 1000000;        //1000000 = 1MHz (1uS per bit)

    spi_cs_fd = open(device, O_RDWR);
    if (spi_cs_fd < 0)
    {
        perror("Error - Could not open SPI device");
        exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0)
    {
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0)
    {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }

    return(status_value);
}



int SpiClosePort (void)
{
    int status_value = -1;

    status_value = close(spi_cs_fd);
    if(status_value < 0)
    {
        perror("Error - Could not close SPI device");
        exit(1);
    }
    return(status_value);
}



int SpiWriteAndRead (unsigned char *data, int length)
//data        Bytes to write.  Contents is overwritten with bytes read.
{
    struct spi_ioc_transfer spi;
    int i = 0;
    int retVal = -1;

    //one spi transfer for each byte
    memset (&spi, 0, sizeof (spi)) ;
    spi.tx_buf        = (unsigned long)data; // transmit from "data"
    spi.rx_buf        = (unsigned long)data; // receive into "data"
    spi.len           = length ;
    spi.delay_usecs   = 0 ;
    spi.speed_hz      = spi_speed ;
    spi.bits_per_word = spi_bitsPerWord ;
    spi.cs_change     = 0;

    for (i = 0 ; i < length ; i++) {
//if(DEBUG)printf("[INPUT] data[%d]=%d(%02x)\n",i,data[i],data[i]);
    }

    retVal = ioctl(spi_cs_fd, SPI_IOC_MESSAGE(1), &spi) ;
    if(retVal < 0) {
    perror("Error - Problem transmitting spi data..ioctl");
    exit(1);
    }

    for (i = 0 ; i < length ; i++) {
//if(DEBUG)printf("[OUTPUT] data[%d]=%d(%02x)\n",i,data[i],data[i]);
    }
    return retVal;
}

int main(int argc, char **argv)
{
    int retCode;
    int i;
    int a2dChannel = 0;  // analog channel
    int a2dVal = 0; 
    float a2dVol = 0; 
    float Vref = 3.3;
#ifdef MCP3002
    unsigned char data[2];
    exit(1)
#endif
#ifdef MCP3204
    unsigned char data[3];
#endif

    if (argc == 2) {
        printf("argv[1]=%s\n",argv[1]);
        a2dChannel=atoi(argv[1]);
    }

    retCode=SpiOpenPort("/dev/spidev0.0"); // ChipSelectはGPIO8
//  retCode=SpiOpenPort("/dev/spidev0.1"); // ChipSelectはGPIO7
if(DEBUG)printf("SpiOpenPort=%d\n",retCode);

    for(i=0;i<20;i++) {
#ifdef MCP3002
        data[0] = 0b01100000 |( ((a2dChannel & 0x03) << 4));  //  first byte transmitted -> start bit
        data[1] = 0; // second byte transmitted....don't care
if(DEBUG)printf("[MCP3002]data[write]=%02x-%02x\n",data[0],data[1]);
        retCode=SpiWriteAndRead (data,sizeof(data));
if(DEBUG)printf("[MCP3002]SpiWriteAndRead=%d\n",retCode);
if(DEBUG)printf("[MCP3002]data[read]=%02x-%02x\n",data[0],data[1]);
        a2dVal = (data[0]<< 8) & 0b1100000000; //first 2 bit
        a2dVal |=  (data[1] & 0xff);
        a2dVol = (float)a2dVal/1023 * Vref;
#endif
#ifdef MCP3204
        data[0] = 0b00000110 |( ((a2dChannel & 0x04) >> 2));  //  first byte transmitted -> start bit -> (SGL/DIF = 1, D2=0)
        data[1] = 0b00000000 |( ((a2dChannel & 0x03) << 6)); // second byte transmitted -> (D1=D0=0)
        data[2] = 0; // third byte transmitted....don't care
if(DEBUG)printf("[MCP3004]data[write]=%02x-%02x-%02x\n",data[0],data[1],data[2]);
        retCode=SpiWriteAndRead (data,sizeof(data));
if(DEBUG)printf("[MCP3004]SpiWriteAndRead=%d\n",retCode);
if(DEBUG)printf("[MCP3004]data[read]=%02x-%02x-%02x\n",data[0],data[1],data[2]);
        a2dVal = (data[1]<< 8) & 0b111100000000; //first 4 bit
        a2dVal |=  (data[2] & 0xff);
        a2dVol = (float)a2dVal/4095 * Vref;
#endif
        printf("a2dVal=%d\n",a2dVal);
        printf("a2dVol=%f[V]\n",a2dVol);
        //sleep(1);
    }

    retCode=SpiClosePort( );
if(DEBUG)printf("SpiClosePort=%d\n",retCode);
    exit(0);
}
