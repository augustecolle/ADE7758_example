#define AVRMS 0x0D
#define BVRMS 0x0E
#define CVRMS 0x0F

#define STATUS 0x19
#define RSTATUS 0x1A

#define ZXA 9
#define ZXB 10
#define ZXC 11

#define PHASE_A 0

class ADE7758{
  public:
    ADE7758(int _CS);
    void begin();
	long getVRMS(char phase);
    
    //Later in private te plaatsen
    unsigned char read8bits(char reg);
    unsigned int read16bits(char reg);
    unsigned long read24bits(char reg);
	
  private:
    int CS;
		
    void enable();
    void disable();
    long getInterruptStatus();
    long getResetInterruptStatus();
};
