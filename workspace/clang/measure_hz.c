#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VCC_REF 3240

uint16_t calcmVolt(uint16_t val)
{
	uint16_t mVolt;
	mVolt = (uint16_t) (((uint32_t) val * VCC_REF) >> 12);
        // val 0x00 0x00 = 4bit x 2 + 4bit x 2 = 16bit
	//12 bit shift (sum = VCC_REF nize)
	return mVolt;
}


int main(int arc, char **argv) 
{
  int number = 1000000;
  double sec;
  double sps;
  FILE *fp;
  char fname[] = "source_test.csv";

  uint16_t* array_ch0 = (uint16_t*)malloc(sizeof(uint16_t) * number);
  uint16_t* array_ch1 = (uint16_t*)malloc(sizeof(uint16_t) * number);

  char out_ch0[] = { 0b00000110, 0b00000000, 0b00000000 };
  char ch0_data[] = { 0x00, 0x00, 0x00 };
  char out_ch1[] = { 0b00000110, 0b01000000, 0b00000000 };
  char ch1_data[] = { 0x00, 0x00, 0x00 };
  
  union voltage 
  {
  	uint16_t value;
  	struct {
  		uint8_t low;
  		uint8_t hi;
  	};
  } volt;	
  
  if(!bcm2835_init())
    return 1;
	
  bcm2835_spi_begin();
  clock_t start,end;
  start = clock();
  for (int i = 0; i < number; i = i + 1) {
      bcm2835_spi_transfernb(out_ch0, ch0_data, 3);
      volt.low = ch0_data[2];
      volt.hi = ch0_data[1];
      array_ch0[i] = volt.value;

      bcm2835_spi_transfernb(out_ch1, ch1_data, 3);
      volt.low = ch1_data[2];
      volt.hi = ch1_data[1];
      array_ch1[i] = volt.value;

  }
  end = clock();
  sec = (double) (end-start) / CLOCKS_PER_SEC;
  sps = (double) number / sec/ (double) 1000;
  printf("%.2f Ksps(2ch)",sps);



  bcm2835_spi_transfernb(out_ch0, ch0_data, 3);
  printf("\nRead data on CH0:    %02X %02X %02X\n", ch0_data[0], ch0_data[1], ch0_data[2]);
  bcm2835_spi_transfernb(out_ch1, ch1_data, 3);
  printf("Read data on CH1:    %02X %02X %02X\n\n", ch1_data[0], ch1_data[1], ch1_data[2]);

  bcm2835_spi_end();
  bcm2835_close();

  //open file
  if( (fp = fopen( fname, "w" )) == NULL ) {
        printf( "could not open the file\n" );
        exit( 1 );
  }
  for(int i = 0; i < number; i++ ) {
        fprintf( fp, "%04X,%04X\n"
           , array_ch0[i], array_ch1[i]);
  }
  printf( "save as %s \n", fname );
  fclose( fp );



  free(array_ch0);
  free(array_ch1);

  return 0;
}
