#ifndef __ClkDetector_h
#define __ClkDetector_h

class ClkDetector {
	
private:

	bool trigReset;
	bool trigResetm1;
	bool trig;
	bool trigm1;
	bool tempoJump;
	bool rst;
	bool clockin;
	bool switchin;
	unsigned int blockIncr;
	unsigned int blockDelay;
	unsigned int overSampIncr;
	unsigned int trigCnt;
	unsigned int mode;
	unsigned int modem1;
	float bpm;
	float blockBpm;
	float sr;
	int blockSize;
	
public:
	
	ClkDetector();
	ClkDetector(float sr0, int blockSize0);
	~ClkDetector();
	
	bool reset(float resetInput, float trigThreshold);
	int setClk(float clkInput, float trigThreshold);
	int setClk(float clkInput, float trigThreshold, unsigned int blkOverSamp);
	
	float getBpm();
	int getMode();
	bool isReset();
	bool isClockin();
	bool isModeSwitchin();
  
};
	
	
inline bool ClkDetector::isReset() {
	return rst;
}
	
	
inline bool ClkDetector::isClockin() {
	return clockin;
}

inline float ClkDetector::getBpm() { 		// To DO : BPM within a range
	return bpm;
}

inline int ClkDetector::getMode() { 		
	return mode;
}
	
	
inline bool ClkDetector::isModeSwitchin() {
	if (mode != modem1) {
		switchin = true;
	}
	else {
		switchin = false;
	}
	return switchin;
}
	
	

	

#endif // __ClkDetector_h__
