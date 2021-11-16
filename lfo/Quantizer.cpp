#ifndef __Quantizer_cpp
#define __Quantizer_cpp

#include "Quantizer.h"
/* TO DO : 
 * 
 * harmonic scaling
 * 
 * setScale -- default would be no scale
 * setKey -- listenable on left ch
 * 
*/



/* Quantizer
 * 
 * Syncable quantizer
 * Using high-speed rates at high BPM may not give a precise result unless process blocks get shorten
 * 
 * Order to call the members :
 * 
 * reset
 * setRate
 * setBpm
 * setClk
 * offsetPhase
 * quantize
 * updatePhase
 * 
*/

Quantizer::Quantizer() {	
	out = 0;									
    bpm = 240;
    phase = 0;
    phasem1 = 0;
    phaseOffs = 0;
    rateNum = 1;
    rateDen = 1;
    rate = 1;
    ratem1 = 1;
	sr = 48000;
	blockSize = 256;
}

Quantizer::Quantizer(float sr0, int blockSize0) {	
	out = 0;									
    bpm = 240;
    phase = 0;
    phasem1 = 0;
    phaseOffs = 0;
    rateNum = 1;
    rateDen = 1;
    rate = 1;
    ratem1 = 1;
	sr = sr0;
	blockSize = blockSize0;
}


Quantizer::~Quantizer() {	
}


float Quantizer::quantize(float in) {  
	double quantPhase = phase + phaseOffs;
	if (quantPhase >= 1) {
		quantPhase -= 1;
	}
	if (quantPhase <0) {
		quantPhase += 1;
	}	 
	double quantPhasem1 = phasem1 + phaseOffs;
	if (quantPhasem1 >= 1) {
		quantPhasem1 -= 1;
	}
	if (quantPhasem1 <0) {
		quantPhasem1 += 1;
	}				
	if (quantPhase - quantPhasem1 < 0) {
		out = in;
	}
	
	return out;
}
	

void Quantizer::updatePhase() {  
	
	phasem1 = phase;
	phase += (double)bpm * (double)blockSize / (double)60 / (double)sr / (double)rate;
	
	if (phase >= 1) {
		phase -= 1;
	}
	
	ratem1 = rate;
}
    
    
void Quantizer::setClk(bool clockin) {  
	
	if(ratem1 != rate) {
		rateDen = 0;
		do
			rateDen++;
		while (modNullApprox(rateDen, 1 / rate) != 0);
		rateNum = (int)(rate * rateDen + 0.5);
	}
	
if (clockin) {
	if (rate <= 1 && rateNum == 1) {
		phase = 0;
	}
	else {
		int phaseNum = (int)(phase * rateNum + 0.5);

		if (rate < 1) {
			phase = (double)phaseNum / (double)rateDen;

			if (phase >= 1) {
				phase = 0;			// or -= 1 ?
			}
		}
		else {
			phase = (double)phaseNum / (double)rateNum;

			if (phase >= 1) {
				phase = 0;
			}
		}
	}
}
}		    

float Quantizer::modNullApprox(float numerator, float denominator) {
	float modulo = numerator/denominator;
	modulo -= (int)modulo;
	if (modulo > 0.9999 || modulo < 0.0001) {
		modulo = 0;
	}
	
	return modulo;
}    


#endif   // __Quantizer_cpp__
