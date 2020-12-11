#include <bcm2835.h>
#include <stdio.h>

#define VCC_REF 3240

uint16_t calcmVolt(uint16_t val)
{
	uint16_t mVolt;
	mVolt = (uint16_t) (((uint32_t) val * VCC_REF) >> 12);
	return mVolt;
}

float calcTemp(uint16_t val)
{
	float temp;
	temp = (calcmVolt(val) - 500) / 10.0;
	return temp;
}

int main(int arc, char **argv) 
{

  char out_ch0[] = { 0b00000110, 0b00000000, 0b00000000 };
	char ch0_data[] = { 0x00, 0x00, 0x00 };
  char out_ch1[] = { 0b00000110, 0b01000000, 0b00000000 };
	char ch1_data[] = { 0x00, 0x00, 0x00 };

	union temperatur 
	{
		uint16_t value;
		struct {
			uint8_t low;
			uint8_t hi;
		};
	} temp;	
  
	if(!bcm2835_init())
		return 1;
	
	bcm2835_spi_begin();

  bcm2835_spi_transfernb(out_ch0, ch0_data, 3);
	printf("\nGelesene Daten an CH0:    %02X %02X %02X\n", ch0_data[0], ch0_data[1], ch0_data[2]);
	bcm2835_spi_transfernb(out_ch1, ch1_data, 3);
	printf("Gelesene Daten an CH1:    %02X %02X %02X\n\n", ch1_data[0], ch1_data[1], ch1_data[2]);

	bcm2835_spi_end();
  bcm2835_close();

  temp.low = ch0_data[2];
	temp.hi = ch0_data[1];
	printf("Analoger Messwert an CH0: %i\n", temp.value);
	printf("Berechnete Spannung:      %i mV\n\n", calcmVolt(temp.value));


  temp.low = ch1_data[2];
	temp.hi = ch1_data[1];
	printf("Analoger Messwert an CH1: %i\n", temp.value);
	//printf("Berechnete Temperatur:    %0.1f C\n\n", calcTemp(temp.value));
	printf("Berechnete Spannung:      %i mV\n\n", calcmVolt(temp.value));

	return 0;
}
