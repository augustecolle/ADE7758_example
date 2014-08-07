#include "Arduino.h"
#include <SPI.h>
#include "ADE7758.h"
#include <avr/wdt.h>
#include "math.h"

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

void ADE7758::write24(char reg, unsigned long data){
        //METHODE NOG NIET GETEST
        enable();
        unsigned char data0=0,data1=0,data2=0;

        reg |= WRITE;

        data0 = (unsigned char)data;
        data1 = (unsigned char)(data>>8);
        data2 = (unsigned char)(data>>16);
        //float = 32 bit, met de 8 MSBs van data wordt dus geen rekening
        //gehouden
        delayMicroseconds(50);
        SPI.transfer((unsigned char)reg);    
        delayMicroseconds(50);    
        SPI.transfer((unsigned char)data2);
        delayMicroseconds(50);
        SPI.transfer((unsigned char)data1);  
        delayMicroseconds(50);
        SPI.transfer((unsigned char)data0);
        delayMicroseconds(50);
        disable();
}

//To minimize noise synchronize the reading with the zero crossing
long ADE7758::getVRMS(char phase){
    int N = 20; //aantal lezingen voor gemiddelde waarde
    unsigned long VRMS = 0;
    long lastupdate = 0;
    for (int i = 0; i<N; i++){
        getResetInterruptStatus(); //clear interrupts
        lastupdate = millis();
        while(!(getInterruptStatus() & (ZXA<<phase))){ //Fase afhankelijk gemaakt, mask shiften met 0,1 of 2
            //Wait for the selected interrupt (zero crossing interrupt)
            if((millis()-lastupdate)>100){
                Serial.println("VRMS Timeout: NaN");
                            return -1;
            }
        }
        VRMS += read24bits(AVRMS+phase);
    }
    return VRMS/N; //Fase afhankelijk gemaakt, register AVRMS+0,1 of 2.
}

long ADE7758::getIRMS(char phase){
    int N = 20; //aantal lezingen voor gemiddelde waarde
    unsigned long IRMS = 0;
    long lastupdate = 0;
    for (int i = 0; i<N; i++){
        getResetInterruptStatus(); //clear interrupts
        lastupdate = millis();
        while(!(getInterruptStatus() & (ZXA<<phase))){ //Fase afhankelijk gemaakt, mask shiften met 0,1 of 2
            //Wait for the selected interrupt (zero crossing interrupt)
            if((millis()-lastupdate)>100){
                Serial.println("IRMS Timeout: NaN");
                            return -1;
            }
        }
        IRMS += read24bits(AIRMS+phase);
    }
    return IRMS/N; //Fase afhankelijk gemaakt, register AIRMS+0,1 of 2.
}

void ADE7758::calibrateOffset(char phase){
    //zie datasheet pagina 55    
    write8(LCYCMODE, 0x38); //zero crossing voor elke fase
    write24(MASK, 0xE00); //enable de interrupts gegenereerd wanneer er een zero crossing is

    int Vnom = 400/math.sqrt(3); //[V] aan te passen per toepassing
    int Vfsc = 260; //[V] full scale spanning, we meten fasespanningen dus 230V +- 23V + marge
    int Vmin = Vfsc/20;
    int VRMSmin = 82246; //[-] meting bij Vmin, rauwe data
    int VRMSnom = 1134765; //[-] meting bij Vnom, rauwe data

    int Itest = 10; //[A] aan te passen per calibratie
    int IRMStest = 0; //[A] meting bij Itest
    int IRMStestmin = 0; //[A] meting bij Itest/500

    int Voffset = 1/64*(Vnom*VRMSmin-Vmin*VRMSnom)/(Vmin-Vnom);
    int Ioffset = 1/16384*(Itest*Itest*IRMStest*IRMStest - (Itest*Itest/250000)*IRMStestmin*IRMStestmin)/((1-500*500)*Itest*Itest/250000);
    
    write8(AVRMSOS+phase, Voffset); //registers volgen op elkaar, dus +phase selecteerd automatisch het juiste register
    write8(AIRMSOS+phase, Ioffset);
}
