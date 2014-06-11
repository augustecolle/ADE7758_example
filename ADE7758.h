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
};
