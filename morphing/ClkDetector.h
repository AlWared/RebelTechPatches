
#ifndef __ClkDetector_h__
#define __ClkDetector_h__

class ClkDetector {
	
private:

	bool trig;
	bool trigm1;
	bool tempoJump;
	bool phaseTo0;
	unsigned int blockIncr;
	unsigned int blockDelay;
	unsigned int overSampIncr;
	unsigned int trigCnt;
	float bpm;
	float blockBpm;
	float sr;
	float blockSize;
	
public:
	
	ClkDetector();
	ClkDetector(float sr0, int blockSize0);
	~ClkDetector();
	
	int setClk(float clkInput, float trigThreshold);
	int setClk(float clkInput, float trigThreshold, unsigned int blkOverSamp);
	float getBpm();
	bool isPhase0();
  
};
  

	
	
inline float ClkDetector::getBpm() { 		// To DO : BPM within a range
	return bpm;
}
	
	
inline bool ClkDetector::isPhase0() {
	return phaseTo0;
}
	

#endif // __ClkDetector_h__
