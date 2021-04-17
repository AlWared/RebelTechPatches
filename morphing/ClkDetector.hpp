
#ifndef __ClkDetector_hpp__
#define __ClkDetector_hpp__

class ClkDetector {
private:
	bool trig;
	bool trigm1;
	bool tempoJump;
	bool phaseTo0;
	unsigned int errCounter;
	unsigned int blockIncrM1;
	unsigned int blockIncr;
	unsigned int fourthBlockIncr;
	unsigned int trigCnt;
	float BPM;
	float blockBPM;
	float sr;
	float sr0;
	float blockSize0;
	float blockSize;
public:
  ClkDetector(float sr0, float blockSize0) {
	BPM= 240;
	blockBPM = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	phaseTo0 = false;
	errCounter= 0;
	blockIncrM1= 0;
	blockIncr= 0;
	fourthBlockIncr= 0;
	trigCnt = 0;
	sr = 48000;
	blockSize = 256;
	
	int setCLK(float clkInput, float trigThreshold);
	float getBPM();
	bool isPhase0();
  }

int setCLK(float clkInput, float trigThreshold) {
		
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
				fourthBlockIncr++;			// TO DO : reset this variable at the right time
				if (fourthBlockIncr > 3) {
					blockIncr++;
					fourthBlockIncr = 0;
				}
				phaseTo0 = false;
			}
		}
	}
	else {
		fourthBlockIncr++;
		if (fourthBlockIncr > 3) {
			blockIncr++;
			fourthBlockIncr = 0;
		}
		phaseTo0 = false;
	}
	
		
	blockIncrM1++;
	trigm1 = trig;
	
	float meanBPM = BPM;
	return (blockIncrM1);
}
	
	
float getBPM() { 		// To DO : BPM within a range
	return BPM;
}
	
	
bool isPhase0() {
	return phaseTo0;
}
		
};

#endif // __ClkDetector_hpp__
