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


void ADE7758::write8(char reg, unsigned char data){
        enable();
        unsigned char data0 = 0;

        // 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
        // For Write -> DB7 = 1  / For Read -> DB7 = 0
        reg |= WRITE;
        data0 = (unsigned char)data;
        
        delayMicroseconds(50);
        SPI.transfer((unsigned char)reg);          //register selection
        delayMicroseconds(50);
        SPI.transfer((unsigned char)data0);
        delayMicroseconds(50);
        disable();
}


void ADE7758::write16(char reg, unsigned int data){
        enable();
        unsigned char data0=0,data1=0;
        // 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
        // For Write -> DB7 = 1  / For Read -> DB7 = 0
        reg |= WRITE;
        //split data
        data0 = (unsigned char)data;
        data1 = (unsigned char)(data>>8);
        
        //register selection, we have to send a 1 on the 8th bit to perform a write
        delayMicroseconds(50);
        SPI.transfer((unsigned char)reg);    
        delayMicroseconds(50);    
        //data send, MSB first
        SPI.transfer((unsigned char)data1);
        delayMicroseconds(50);
        SPI.transfer((unsigned char)data0);  
        delayMicroseconds(50);
        disable();
}

//is voorlopig voor A geprogrammeerd!
//To minimize noise synchronize the reading with the zero crossing
//Auguste denkt: als er een interrupt wordt gegenereerd omdat fase A een
//zerocrossing doet moet ge op dat moment de uitlezing doen. Als de
//zerocrossing te lang duurt doet ge een reset van de processor met behulp van
//wtd (de processor is bezig met te wachten op de zerocrossing en heeft een
//hulp van buitenaf nodig om te onderbreken).
long ADE7758::getVRMS(char phase){
    long lastupdate = 0;
    getResetInterruptStatus(); //clear interrupts
    lastupdate = millis();
    while(!(getInterruptStatus() & (ZXA<<phase))){ //Fase afhankelijk gemaakt, mask shiften met 0,1 of 2
        //Wait for the selected interrupt (zero crossing interrupt)
        if((millis()-lastupdate)>100){
            Serial.println("VRMS Timeout");
            break;
        }
    }
    return read24bits(AVRMS+phase); //Fase afhankelijk gemaakt, register AVRMS+0,1 of 2.
}
