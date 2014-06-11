#define PHASE_A 0

class ADE7758{
  public:
    ADE7758(int _CS);
    void begin();
	long getVRMS(char phase);
    unsigned char read8bits(char reg);
	
  private:
    int CS;
		
    void enable();
    void disable();
};
