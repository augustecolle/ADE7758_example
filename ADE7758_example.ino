#include "ADE7758.h"
#include <SPI.h>

ADE7758 myADE(10);

void setup(){
  Serial.begin(9600);
  myADE.begin();
  
  //myADE.write16(0x,);
}

void loop(){
  Serial.println(myADE.read8bits(0x11),HEX);
  Serial.println(myADE.getVRMS(PHASE_A)/4912.68);
  delay(1000);
}
