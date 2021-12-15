#ifndef __Quantizer_cpp__
#define __Quantizer_cpp__

#include "Quantizer.h"

/* TO DO : 
 * 
 * harmonic scaling
 * 
 * 
*/



/* Quantizer
 * 
 * Syncable quantizer
 * Using high-speed rates at high BPM may not give a precise result unless process blocks get shorten
 * 
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
	shapeSel = 0;
	shapeStart = 1;
	scaleSel = 0;
	scaleKey = 440;
	holdValue = 0;
	goalValue = 0;
    phase = 0;
    phasem1 = 0;
    phaseOffs = 0;
    rateNum = 1;
    rateDen = 1;
    rate = 1;
    ratem1 = 1;
	sr = 48000;
	blockSize = 256;
	
	//scaler = new Scaler(scaleKey, 0.0, 10.0);
}

Quantizer::Quantizer(float sr0, int blockSize0, float scaleKey0) {	
	out = 0;									
    bpm = 240;
	shapeSel = 0;
	shapeStart = 1;
	scaleSel = 0;
	scaleKey = scaleKey0;
	holdValue = 0;
	goalValue = 0;
    phase = 0;
    phasem1 = 0;
    phaseOffs = 0;
    rateNum = 1;
    rateDen = 1;
    rate = 1;
    ratem1 = 1;
	sr = sr0;
	blockSize = blockSize0;
	
	//scaler = new Scaler(scaleKey, 0.0, 10.0);
}


Quantizer::~Quantizer() {	
	//delete scaler;
}


float Quantizer::getOut() {
	
	double quantPhase = phase + phaseOffs + 1.0;
	
	quantPhase = quantPhase - (double)((int)quantPhase);
	
	//if (quantPhase >= 1) {
		//quantPhase -= 1;
	//}
	//if (quantPhase <0) {
		//quantPhase += 1;
	//}
	
	if(quantPhase < shapeStart) {
		out = holdValue;
	}
	else {
		if(shapeSel < 0.1) {			// brick shape
			out = holdValue;
		}
		else if(shapeSel < 0.4) {		// brick to exponential
			out = (1.0 - (shapeSel - 0.1) / 0.3) * holdValue + ((shapeSel - 0.1) / 0.3) * holdValue * exp((1 / (1 - shapeStart)) * log(goalValue / holdValue) * (quantPhase - shapeStart));
		}
		else if(shapeSel < 0.7) {		// exp to linear
			out = (1.0 - (shapeSel - 0.4) / 0.3) * exp((1 / (1 - shapeStart)) * log(goalValue / holdValue) * (quantPhase - shapeStart)) + ((shapeSel - 0.4) / 0.3) * (goalValue * (quantPhase - shapeStart) / (1.0 - shapeStart) + holdValue * (1.0 - quantPhase) / (1.0 - shapeStart));
		}
		else {							// linear to sigmoid (which is not really sigmoid)
			out = goalValue + (holdValue - goalValue) / (1 + pow(((1 - shapeStart) / (quantPhase - shapeStart) - 1), (-1 - (shapeSel - 0.7) * 10)));		// maybe would need a quadratic coeff to keep more linear change
		}
		// TODO: if exp is too costy, change it to half "fake" sigmoid
		// exp : holdValue*exp((1/(1-shapStart))*log(goalValue/holdValue)*(quantPhase-shapeStart))
		// lin : (goalValue * (quantPhase - shapeStart) / (1.0 - shapeStart) + holdValue * (1.0 - quantPhase) / (1.0 - shapeStart))
		// "fake" sig : goalValue + (holdValue - goalValue) / (1 + ((1 - shapeStart) / (quantPhase - shapeStart) - 1)^(-1-(shapeSel-0.7)*10))
	
	}
	return out;
}


bool Quantizer::isQuantizing() {  
	
	double quantPhase = phase + phaseOffs + 1.0;
	double quantPhasem1 = phasem1 + phaseOffs + 1.0;
	
	quantPhase = quantPhase - (double)((int)quantPhase);
	quantPhasem1 = quantPhasem1 - (double)((int)quantPhasem1);
	
	
	//if (quantPhase >= 1) {
		//quantPhase -= 1;
	//}
	//if (quantPhase <0) {
		//quantPhase += 1;
	//}	 
	
	//if (quantPhasem1 >= 1) {
		//quantPhasem1 -= 1;
	//}
	//if (quantPhasem1 <0) {
		//quantPhasem1 += 1;
	//}		
			
	if (quantPhase - quantPhasem1 < 0) {
		return true;
	}
	else {
		return false;
	}
}
	

void Quantizer::updatePhase() {  
	
	phasem1 = phase;
	phase += (double)bpm * (double)blockSize / (double)60.0 / (double)sr / (double)rate;
	
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
		phase = 0.0;
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

void Quantizer::setKey(float keyFreq0) {
	scaleKey = keyFreq0;
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
