#include "Arduino.h"
#include <SPI.h>
#include "ADE7758.h"
#include <avr/wdt.h>

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

long ADE7758::getInterruptStatus(){
    return read24bits(STATUS);
}

long ADE7758::getResetInterruptStatus(){
    return read24bits(RSTATUS);
}

unsigned char ADE7758::read8bits(char reg){
	enable();
    unsigned char b0;
	delayMicroseconds(50);
	SPI.transfer(reg);
	delayMicroseconds(50);
	b0=SPI.transfer(0x00);
	delayMicroseconds(50);
    disable();
	return b0;
}

unsigned int ADE7758::read16bits(char reg){
    enable();
    unsigned char b1,b0;
    delayMicroseconds(50);
    SPI.transfer(reg);
    delayMicroseconds(50);
    b1=SPI.transfer(0x00);
    delayMicroseconds(50);
    b0=SPI.transfer(0x00);
    delayMicroseconds(50);
    disable();
    return (unsigned int)b1<<8 | (unsigned int)b0;
}

unsigned long ADE7758::read24bits(char reg){
    enable();
    unsigned char b2,b1,b0;
    delayMicroseconds(50);
    SPI.transfer(reg);
    delayMicroseconds(50);
    b2=SPI.transfer(0x00);
    delayMicroseconds(50);
    b1=SPI.transfer(0x00);
    delayMicroseconds(50);
    b0=SPI.transfer(0x00);
    delayMicroseconds(50);
    disable();
    return (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
}

//is voorlopig voor A geprogrammeerd!
//To minimize noise synchronize the reading with the zero crossing
long ADE7758::getVRMS(char phase){
    long lastupdate = 0;
    getResetInterruptStatus(); //clear interrupts
    lastupdate = millis();
    while(!(getInterruptStatus() & (ZXA))){ //Nog fase-afhankelijk maken! fout in andere library? Maar hoe corrigeren?
        //Wait for the selected interrupt (zero crossing interrupt)
        if((millis()-lastupdate)>100){
            wdt_reset(); //Betekenis hiervan?? waarom reset je de watchdogtimer?
            Serial.println("VRMS Timeout");
            break;
        }
    }
    return read24bits(AVRMS); //Nog fase-afhankelijk maken, zelfde opmerking
}
