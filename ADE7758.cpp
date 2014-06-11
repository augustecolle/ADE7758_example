#include "Arduino.h"
#include <SPI.h>
#include "ADE7758.h"

ADE7758::ADE7758(int _CS){
	CS = _CS;
}

void ADE7758::begin(){
	pinMode(CS,OUTPUT);
	digitalWrite (CS,HIGH);
	SPI.setDataMode(SPI_MODE2);
	SPI.setClockDivider(SPI_CLOCK_DIV32);
	SPI.setBitOrder(MSBFIRST);
	SPI.begin();
	delay(10);
}

void ADE7758::enable(){
	digitalWrite(CS,LOW);
}

void ADE7758::disable(){
	digitalWrite(CS,HIGH);
}

unsigned char ADE7758::read8bits(char reg){
	unsigned char ret;
	delayMicroseconds(50);
	SPI.transfer(reg);
	delayMicroseconds(50);
	ret=SPI.transfer(0x00);
	delayMicroseconds(50);
	delay(50);
	return ret;
}

long ADE7758::getVRMS(char phase){
	
}
