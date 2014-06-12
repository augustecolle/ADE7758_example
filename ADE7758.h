//voor als de library per ongeluk twee keer wordt geimporteerd
#ifndef ADE7758_h
#define ADE7758_h
#include "Arduino.h"    

#define WRITE 0x80 //0x80 = b10000000 voor te schrijven moet BT7 1 zijn

#define AVRMS 0x0D
#define BVRMS 0x0E
#define CVRMS 0x0F

#define STATUS 0x19
#define RSTATUS 0x1A

#define ZXA 0x0200 //waar haalt ge dat RIEN? ziet er mij geen valid register uit. Dat moet de zero crossing zijn voor fase A. Edit: gevonden, tmoet een mask zijn want ge doet een & operator in uw functie voor VRMSA. In de ADE7758 bit positie 9 dus 0x0200
#define ZXB 0x0400
#define ZXC 0x0800

#define PHASE_A 0
#define PHASE_B 1
#define PHASE_C 2

class ADE7758{
  public:
    ADE7758(int _CS);
    void begin();
	long getVRMS(char phase);
    
    //Later in private te plaatsen
    unsigned char read8bits(char reg);
    unsigned int read16bits(char reg);
    unsigned long read24bits(char reg);
    
    //write methodes, later ook in private plaatsen
    void write8(char reg, unsigned char data);
    void write16(char reg, unsigned int data);
	
  private:
    int CS;
		
    void enable();
    void disable();
    long getInterruptStatus();
    long getResetInterruptStatus();
};


#endif
