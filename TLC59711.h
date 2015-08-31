/*************************************************** 
  This is a library for our Adafruit 12-channel PWM/LED driver

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

#ifndef _ADAFRUIT_TLC59711_H
#define _ADAFRUIT_TLC59711_H

#include "application.h"

class TLC59711 {
 public:
  
  //Constructor - Create a TLC59711 object and allocate storage
  //`numDaisyChainedICs` represents number of daisy-chained TLC59711s
  TLC59711(uint8_t numDaisyChainedICs);
  
  //Calls SPI.begin() - must be called only once
  void begin(void);

  //Set intensity, between 0-100% for a PWM channel as a floating point number
  void setIntensity(uint8_t chan, float intensity);

  //Must be called in order to transfer new PWM values to ICs
  //Initiates a batch transfer of PWM values for all channels to the ICs
  void transferToICs(void);

  protected:

  //directly set the PWM duty cycle of a PWM channel
  void setPWM(uint8_t chan, uint16_t pwm);

  //Buffer for storing PWM values for each channel before transfer to ICs
  uint16_t * pwmbuffer;

  //Consts determined at runtime
  const uint8_t numICs;
  const unsigned int highestChannel; 
  //Command used to indicate we're about to write and set modes
  const uint32_t writeCommand;
  //Scaling factor for 100% to max PWM = 65535 / 100%
  static const float scalingFactor;
};

#endif
