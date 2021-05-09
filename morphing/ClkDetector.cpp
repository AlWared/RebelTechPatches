#include "ClkDetector.h"


/* ClkDetector
 * 
 * 3 mode tempo utility
 * Default mode when no clk in: bpm = 30 and phaseTo0 turned true for every new cycle
 * Slave tempo detection whenever a clk input cross the threshold
 * Master tempo mode when adjusting the clkInput above the threshold value
*/

ClkDetector::ClkDetector() {										
    bpm= 240;
	blockBpm = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	phaseTo0 = false;
	blockIncr= 0;
	blockDelay= 0;
	overSampIncr= 0;
	trigCnt = 0;
	sr = 48000;
	blockSize = 256;
}

ClkDetector::ClkDetector(float sr0, int blockSize0) {										
    bpm= 240;
	blockBpm = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	phaseTo0 = false;
	blockIncr= 0;
	blockDelay= 0;
	overSampIncr= 0;
	trigCnt = 0;
	sr = sr0;
	blockSize = blockSize0;
}


ClkDetector::~ClkDetector() {	
}


// setClk
//
// Calculates the tempo according to samplerate and blockSize
// Entering slave tempo detection when trig is set true, bpm is a mean value to correct the block error
// bpm and phase refreshed when tempo jumps enough
// Entering default mode after 6 seconds whithout trigs
// Entering master tempo mode when trig remains true
//
int ClkDetector::setClk(float clkInput, float trigThreshold) {
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {		// clock detection mode
		tempoJump = (blockIncr - 60*sr/blockSize/blockBpm)*(blockIncr - 60*sr/blockSize/blockBpm)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			bpm = ((trigCnt-1)*blockBpm + (60*sr/blockSize/blockIncr)) / trigCnt; // mean bpm
			//phase = 0;			// in CLK mode, phase only forced to 0 when "mean bpm" is calculated
			phaseTo0 = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBpm = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {			// Default mode
		if(trig == false) {									// 6sec without trig
			bpm = 60/(6-4);				// default bpm
			//phase = 0;
			phaseTo0 = true;								
			blockIncr = (4*sr/blockSize)-1;
		}
		else {												// master clock mode
			blockDelay = sr / blockSize / 4 / clkInput;		// int part to calculate an even number a blocks 
			bpm = 60 * sr / blockSize / blockDelay;			
			if (blockIncr > (6*sr/blockSize + blockDelay)) {	
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
	
	return (trigCnt);	// for debug
}


int ClkDetector::setClk(float clkInput, float trigThreshold, unsigned int blkOverSamp) {	// blkOverSamp to be used when an audio block is called many times
		
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {
		tempoJump = (blockIncr - 60*sr/blockSize/blockBpm)*(blockIncr - 60*sr/blockSize/blockBpm)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			bpm = ((trigCnt-1)*blockBpm + (60*sr/blockSize/blockIncr)) / trigCnt; // mean bpm
			//phase = 0;			// in CLK mode, phase only forced to 0 when "mean bpm" is calculated
			phaseTo0 = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBpm = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {						// 6sec without trig
		if(trig == false) {
			bpm = 60/(6-4);	
			//phase = 0;
			phaseTo0 = true;								// default bpm
			blockIncr = (4*sr/blockSize)-1;
		}
		else {
			bpm = 60 * sr / blockSize / ((int) (sr / blockSize / 4 / clkInput));
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
		if (overSampIncr > blkOverSamp-1) {		// downsampling
			blockIncr++;
			overSampIncr = 0;
		}
		phaseTo0 = false;
	}
	
		
	trigm1 = trig;
	
	return (trigCnt);  	// for debugging
}
