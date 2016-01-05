/*************************************************** 
  This is a library for our Adafruit 24-channel PWM/LED driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1455

  These drivers uses SPI to communicate, 3 pins are required to  
  interface: Data, Clock and Latch. The boards are chainable

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

/*************************************************** 
  Adapted by Michael Siegel @pomplesiegel on 8/31/2015 for 
  use on Particle (Spark) Core and Photon

  Credit to Limor Fried/Ladyada (Adafruit Industries) 
  Adafruit MCP_TLC59711 driver used for algorithm within 
  transferToICs() and configuration in GenerateWriteCommand().
****************************************************/

#include "TLC59711.h"

//Make sure intensity of 100% maps to Max PWM value
const float TLC59711::scalingFactor = 65535.1 / 100.0;

//Helper function (not a class method) to generate a const-able write command 
//to be passed into initializer list of constructor
//No need to ever be called by a human!
static uint32_t GenerateWriteCommand()
{
  //Generate value of `writeCommand`
  // Magic word for write
  uint32_t writeCommand = 0x25;
  writeCommand <<= 5;

  //OUTTMG = 1, EXTGCK = 0, TMGRST = 1, DSPRPT = 1, BLANK = 0 -> 0x16
  writeCommand |= 0x16;
  writeCommand <<= 7;
  writeCommand |= 0x7F; //BCr
  writeCommand <<= 7;
  writeCommand |= 0x7F; //BCg
  writeCommand <<= 7;
  writeCommand |= 0x7F; //BCb
  return writeCommand;
}

//Constructor - Create a TLC59711 object and allocate storage
//`numDaisyChainedICs` represents number of daisy-chained TLC59711s
TLC59711::TLC59711(uint8_t numDaisyChainedICs) : 
  numICs(numDaisyChainedICs), 
  highestChannel( numDaisyChainedICs * 12 - 1 ), 
  writeCommand( GenerateWriteCommand() ) //Call helper method to give const `writeCommand` a value
{
  //allocate buffer to store PWM values for all channels
  pwmbuffer = new uint16_t[ 12 * numICs ];
}

//Must be called in order to transfer new PWM values to ICs
//Initiates a batch transfer of PWM values for all channels to the ICs
void TLC59711::transferToICs(void) 
{ 
  //Disable interrupts. Particle equivalent of android's cli()
  noInterrupts();

  for (uint8_t n=0; n<numICs; n++) 
  {
    SPI.transfer(writeCommand >> 24);
    SPI.transfer(writeCommand >> 16);
    SPI.transfer(writeCommand >> 8);
    SPI.transfer(writeCommand);

    // 12 channels per TLC59711
    for (int8_t c=11; c >= 0 ; c--) 
    {
      // 16 bits per channel, send MSB first
      SPI.transfer(pwmbuffer[n*12+c]>>8);
      SPI.transfer(pwmbuffer[n*12+c]);
    }
  }

  //Enable interrupts. Particle equivalent of android's sei()
  interrupts();
}

//Set intensity, between 0-100% for a PWM channel as a floating point number
bool TLC59711::setIntensity(uint8_t chan, float intensity)
{
  // Serial.println("Chan " + String(chan) + " -> " + String(intensity) + '%' );
  return setPWM(chan, intensity * scalingFactor);
}

//directly set the PWM duty cycle of a PWM channel
bool TLC59711::setPWM(uint8_t chan, uint16_t pwm) {
  if ( chan > highestChannel ) 
  {
    // Serial.println("PWM channel called out of range");
    return false;
  }

  //only write change to array if necessary
  if( pwmbuffer[chan] != pwm)
  {
    pwmbuffer[chan] = pwm;  //write value to buffer for this channel
    return true;
  }
  else
    return false;
}

//Calls SPI.begin() - must be called only once
void TLC59711::begin() 
{
  SPI.begin();
  
  //Configure SPI settings for this IC
  SPI.setBitOrder(MSBFIRST);
  //Photon's SPI master clock generator runs at 60Mhz. 
  //For stability (no flickering) we need the SPI clock 
  //running at < 1Mhz. 60/64 = ~.93Mhz
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE0);
}