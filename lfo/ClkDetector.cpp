#ifndef __ClkDetector_cpp
#define __ClkDetector_cpp

#include "ClkDetector.h"


/* ClkDetector
 * 
 * 3 mode tempo utility
 * Default mode when no clk in: bpm = 30 and clockin turned true for every new cycle
 * Slave tempo detection whenever a clk input cross the threshold
 * Master tempo mode when adjusting the clkInput above the threshold value
*/

ClkDetector::ClkDetector() {										
    bpm= 240;
	blockBpm = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	rst = false;
	clockin = false;
	switchin = false;
	blockIncr= 0;
	blockDelay= 0;
	overSampIncr= 0;
	trigCnt = 0;
	mode = 0;
	sr = 48000;
	blockSize = 256;
}

ClkDetector::ClkDetector(float sr0, int blockSize0) {										
    bpm= 240;
	blockBpm = 240;
	trig= false;
	trigm1= false;
	tempoJump= false;
	rst = false;
	clockin = false;
	switchin = false;
	blockIncr= 0;
	blockDelay= 0;
	overSampIncr= 0;
	trigCnt = 0;
	mode = 0;
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
	
	modem1 = mode;
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {		// clock detection mode
		mode = 2;
		tempoJump = (blockIncr - 60*sr/blockSize/blockBpm)*(blockIncr - 60*sr/blockSize/blockBpm)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			bpm = ((trigCnt-1)*blockBpm + (60*sr/blockSize/blockIncr)) / trigCnt; // mean bpm
			clockin = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBpm = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {			// Default mode
		if(trig == false) {									// 6sec without trig
			mode = 0;
			bpm = 60/(6-4);				// default bpm
			clockin = true;								
			blockIncr = (4*sr/blockSize)-1;
		}
		else {												// master clock mode
			mode = 1;
			blockDelay = sr / blockSize / 4 / clkInput;		// int part to calculate an even number a blocks 
			bpm = 60 * sr / blockSize / blockDelay;			
			if (blockIncr > (6*sr/blockSize + blockDelay)) {	
				blockIncr = 6*sr/blockSize;
				clockin = true;
			}
			else {
				blockIncr++;
				clockin = false;
			}
		}
	}
	else {
		blockIncr++;
		clockin = false;
	}
	
		
	trigm1 = trig;
	
	return (blockIncr);	// for debug
}


int ClkDetector::setClk(float clkInput, float trigThreshold, unsigned int blkOverSamp) {	// blkOverSamp to be used when an audio block is called many times
		
	modem1 = mode;
	
	if(clkInput > trigThreshold) {
		trig = true;
	}
	else trig = false;
	
	if(trig == true && trigm1 == false) {
		mode = 2;
		tempoJump = (blockIncr - 60*sr/blockSize/blockBpm)*(blockIncr - 60*sr/blockSize/blockBpm)>1;
		if(tempoJump && trigCnt>1) {		// tempo jumps when pulse change audio block
			bpm = ((trigCnt-1)*blockBpm + (60*sr/blockSize/blockIncr)) / trigCnt; // mean bpm
			clockin = true;
			trigCnt = 0; 
		}
		trigCnt++;
		blockBpm = 60*sr/blockSize/blockIncr;
		blockIncr = 0;
	}
	else if (blockIncr > ((6*sr/blockSize)-1)) {						// 6sec without trig
		if(trig == false) {
			mode = 0;
			bpm = 60/(6-4);	
			clockin = true;								// default bpm
			blockIncr = (4*sr/blockSize)-1;
		}
		else {
			mode = 1;
			bpm = 60 * sr / blockSize / ((int) (sr / blockSize / 4 / clkInput));
			if (blockIncr > (6*sr/blockSize + ((int) (sr / blockSize / 4 / clkInput)))) {
				blockIncr = 6*sr/blockSize;
				clockin = true;
			}
			else {
				overSampIncr++;			// TO DO : reset this variable at the right time
				if (overSampIncr > blkOverSamp-1) {
					blockIncr++;
					overSampIncr = 0;
				}
				clockin = false;
			}
		}
	}
	else {
		overSampIncr++;
		if (overSampIncr > blkOverSamp-1) {		// downsampling
			blockIncr++;
			overSampIncr = 0;
		}
		clockin = false;
	}
	
		
	trigm1 = trig;
	
	return (trigCnt);  	// for debugging
}


bool ClkDetector::reset(float resetInput, float trigThreshold) {
	
	if(resetInput > trigThreshold) {
		trigReset = true;
	}
	else trigReset = false;
	
	if(trigReset == true && trigResetm1 == false) {
		mode = 2;
		rst = true;
		clockin = true;
		trigCnt = 0;
		blockIncr = 0;
		if (trig == true) {								// 6sec without trig
			mode = 0;
			blockIncr = 6*sr/blockSize;
		}
		else {								// 6sec without trig
			mode = 1;
			blockIncr = (4*sr/blockSize)-1;
		}	
	}
	else {
		rst = false;
	}
	return rst;
}

#endif   // __ClkDetector_cpp__
