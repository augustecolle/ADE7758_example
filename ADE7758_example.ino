#include "ADE7758.h"
#include <SPI.h>

ADE7758 myADE(10);

void setup(){
  Serial.begin(9600);
  myADE.begin();
}

void loop(){
  Serial.println(myADE.read8bits(0x11),HEX);
  Serial.println(myADE.getVRMS(PHASE_A));
  delay(1000);
}
