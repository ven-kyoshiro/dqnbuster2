import sys
import time
import RPi.GPIO as GPIO

class MCP3204(object):
    def __init__(self):
        self.spi_clk  = 11
        self.spi_mosi = 10
        self.spi_miso = 9
        self.spi_ss   = 8
        self.GPIO = GPIO
        self.GPIO.setwarnings(False)
        self.GPIO.setmode(GPIO.BCM)

        self.GPIO.setup(self.spi_mosi, self.GPIO.OUT)
        self.GPIO.setup(self.spi_miso, self.GPIO.IN)
        self.GPIO.setup(self.spi_clk,  self.GPIO.OUT)
        self.GPIO.setup(self.spi_ss,   self.GPIO.OUT)

    def get_value(self):
        values = []
        for ch in range(4): 
            self.GPIO.output(self.spi_ss,   False)
            self.GPIO.output(self.spi_clk,  False)
            self.GPIO.output(self.spi_mosi, False)
            self.GPIO.output(self.spi_clk,  True)
            self.GPIO.output(self.spi_clk,  False)
            
            cmd = (ch | 0x18) << 3

            for i in range(5):
                if (cmd & 0x80):
                    self.GPIO.output(self.spi_mosi, True)
                else:
                    self.GPIO.output(self.spi_mosi, False)
                cmd <<= 1
                self.GPIO.output(self.spi_clk, True)
                self.GPIO.output(self.spi_clk, False)
            self.GPIO.output(self.spi_clk, True)
            self.GPIO.output(self.spi_clk, False)
            self.GPIO.output(self.spi_clk, True)
            self.GPIO.output(self.spi_clk, False)
            value = 0
            for i in range(12):
                value <<= 1
                self.GPIO.output(self.spi_clk, True)
                if (self.GPIO.input(self.spi_miso)):
                    value |= 0x1
                self.GPIO.output(self.spi_clk, False)
            self.GPIO.output(self.spi_ss, True)
            values.append(value)
        return values

if __name__ == "__main__":
    adc = MCP3204()
    print(adc.get_value())
 

    

