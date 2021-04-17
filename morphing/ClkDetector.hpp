
#ifndef __ClkDetector_hpp__
#define __ClkDetector_hpp__

class ClkDetector {
private:
	bool trig;
	bool trigm1;
	bool tempoJump;
	bool phaseTo0;
	unsigned int blockIncr;
	unsigned int overSampIncr;
	unsigned int trigCnt;
	float BPM;
	float blockBPM;
	float sr0;
	int blockSize0;
	float sr;
	float blockSize;
public:
  ClkDetector(float sr0, int blockSize0) {
	BPM= 240;
	blockBPM = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	phaseTo0 = false;
	blockIncr= 0;
	overSampIncr= 0;
	trigCnt = 0;
	sr = 48000;
	blockSize = 256;
	
	void setSrBlkSize(float sr0, int blockSize0);
	int setCLK(float clkInput, float trigThreshold);
	int setCLK(float clkInput, float trigThreshold, unsigned int blkOverSamp);
	float getBPM();
	bool isPhase0();
  }



void setSrBlkSize(float sr0, int blockSize0) {
	
	//sr = sr0;
	//blockSize = blockSize0;
}


int setCLK(float clkInput, float trigThreshold) {
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {
		tempoJump = (blockIncr - 60*sr/blockSize/blockBPM)*(blockIncr - 60*sr/blockSize/blockBPM)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			BPM = ((trigCnt-1)*blockBPM + (60*sr/blockSize/blockIncr)) / trigCnt; // mean BPM
			//phase = 0;			// in CLK mode, phase only forced to 0 when "mean BPM" is calculated
			phaseTo0 = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBPM = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {						// 6sec without trig
		if(trig == false) {
			BPM = 60/(6-4);	
			//phase = 0;
			phaseTo0 = true;								// default BPM
			blockIncr = (4*sr/blockSize)-1;
		}
		else {
			BPM = 60 * sr / blockSize / ((int) (sr / blockSize / 4 / clkInput));
			if (blockIncr > (6*sr/blockSize + ((int) (sr / blockSize / 4 / clkInput)))) {
				blockIncr = 6*sr/blockSize;
				//phase = 0;
				phaseTo0 = true;
			}
			else {
				blockIncr++;
				phaseTo0 = false;
			}
		}
	}
	else {
		blockIncr++;
		phaseTo0 = false;
	}
	
		
	trigm1 = trig;
	
	return (trigCnt);
}
	
	
int setCLK(float clkInput, float trigThreshold, unsigned int blkOverSamp) {
		
	//static int blockIncr = 0;
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {
		tempoJump = (blockIncr - 60*sr/blockSize/blockBPM)*(blockIncr - 60*sr/blockSize/blockBPM)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			BPM = ((trigCnt-1)*blockBPM + (60*sr/blockSize/blockIncr)) / trigCnt; // mean BPM
			//phase = 0;			// in CLK mode, phase only forced to 0 when "mean BPM" is calculated
			phaseTo0 = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBPM = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {						// 6sec without trig
		if(trig == false) {
			BPM = 60/(6-4);	
			//phase = 0;
			phaseTo0 = true;								// default BPM
			blockIncr = (4*sr/blockSize)-1;
		}
		else {
			BPM = 60 * sr / blockSize / ((int) (sr / blockSize / 4 / clkInput));
			if (blockIncr > (6*sr/blockSize + ((int) (sr / blockSize / 4 / clkInput)))) {
				blockIncr = 6*sr/blockSize;
				//phase = 0;
				phaseTo0 = true;
			}
			else {
				overSampIncr++;			// TO DO : reset this variable at the right time
				if (overSampIncr > blkOverSamp-1) {
					blockIncr++;
					overSampIncr = 0;
				}
				phaseTo0 = false;
			}
		}
	}
	else {
		overSampIncr++;
		if (overSampIncr > blkOverSamp-1) {
			blockIncr++;
			overSampIncr = 0;
		}
		phaseTo0 = false;
	}
	
		
	trigm1 = trig;
	
	return (trigCnt);
}
	
	
float getBPM() { 		// To DO : BPM within a range
	return BPM;
}
	
	
bool isPhase0() {
	return phaseTo0;
}
		
};

#endif // __ClkDetector_hpp__
