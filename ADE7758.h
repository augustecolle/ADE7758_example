//voor als de library per ongeluk twee keer wordt geimporteerd
#ifndef ADE7758_h
#define ADE7758_h
#include "Arduino.h"    

#define WRITE 0x80 //0x80 = b10000000 voor te schrijven moet BT7 1 zijn

#define AVRMS 0x0D
#define BVRMS 0x0E
#define CVRMS 0x0F
#define AVRMSOS 0x33 //voor calibratie
#define BVRMSOS 0x34
#define CVRMSOS 0x35
#define AIRMSOS 0x36
#define BIRMSOS 0x37
#define CIRMSOS 0x38

#define LCYCMODE 0x17 //The functionalities involved the line-cycle accumulation mode in the ADE7758 are defined by writing to the LCYCMODE register. Voor de zerocrossings te tellen moet de ZX per fase hoog gezet worden van dit register (voor alle fases 0x38)
#define MASK 0x18 //When an interrupt event occurs in the ADE7758, the IRQ logic output goes active low if the mask bit for this event is Logic 1 in the MASK register.

#define STATUS 0x19
#define RSTATUS 0x1A

#define ZXA 0x0200 
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
    void write24(char reg, unsigned long data);
	
  private:
    int CS;

    void enable();
    void disable();
    void calibrateOffset(char phase);
    long getInterruptStatus();
    long getResetInterruptStatus();
};


#endif
