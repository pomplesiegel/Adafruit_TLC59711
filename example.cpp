#include "application.h"
#include "TLC59711.h"

SYSTEM_MODE(MANUAL);

#include "TLC59711.h"

TLC59711 tlc = TLC59711(1);

void setup() {
  Serial.begin(9600);
  
  Serial.println("TLC59711 test");
  tlc.begin();
}

const float normalizer = 100.0 / 11.0; 

void loop() {
  Serial.println("Top of loop!");

  //Just for example
  //Set each channel (12) of a single TLC59711 to a different duty cycle
  //Channel 0 at 0%, channel 11 at 100%
  for(int i=0; i < 12; i++)
    tlc.setIntensity(i, i * normalizer );
  tlc.transferToICs();
  delay(500);

  //Turn everything off
  for(int i=0; i < 12; i++)
    tlc.setIntensity(i, 0 );
  tlc.transferToICs();
  delay(500);
}


