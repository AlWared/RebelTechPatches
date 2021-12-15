#ifndef __Lfo_cpp
#define __Lfo_cpp

#include "Lfo.h"


/* Lfo
 * 
 * Syncable LFO
 * Using high-speed rates at high BPM may not give a precise result unless process blocks get shorten
 * It is designed to use a full [0-1] range, therefore center is +0.5
 * 
 * 
 * Order to call the members :
 * 
 * reset
 * selectExtWave
 * selectRate
 * setBpm
 * setClk
 * selectLfoType
 * offsetPhase
 * updatePhase
 * 
*/

Lfo::Lfo() {	
	totalRatios = 1;
	for (int idx = 0; idx < numRateSlots; idx++) {    				
        Rates[idx].rate = 1; 				
        Rates[idx].num = 1; 				
        Rates[idx].den = 1;
    }
    for (int WFid=0; WFid<WAVETABLE_SIZE; WFid++) {
		for (int i=0; i<WAVE_LENGTH; i++) {
			wt[WFid][i] = 0.0;
		}
	}
	lfo = 0;									
    bpm = 240;
    phase = 0;
    phaseOffs = 0;
    ratem1 = 1;
    typeSel = 0;
    waveSel = 0;
	sr = 48000;
	blockSize = 256;
}

Lfo::Lfo(float sr0, int blockSize0) {	
	totalRatios = 1;
	for (int idx = 0; idx < numRateSlots; idx++) {    				
        Rates[idx].rate = 1; 				
        Rates[idx].num = 1; 				
        Rates[idx].den = 1;
    }
    for (int WFid=0; WFid<WAVETABLE_SIZE; WFid++) {
		for (int i=0; i<WAVE_LENGTH; i++) {
			wt[WFid][i] = 0.0;
		}
	}
	lfo = 0;									
    bpm = 240;
    phase = 0;
    phaseOffs = 0;
    ratem1 = 1;
    typeSel = 0;
    waveSel = 0;
	sr = sr0;
	blockSize = blockSize0;
}


Lfo::~Lfo() {										
    //for (int idx = 0; idx < numRateSlots; idx++) {
        //float *temp = Rates[idx].rate;
        //if (temp != 0)
            //delete [] temp;
    //}
}

void Lfo::fillRates(double* rate0, int totalRatios0) {  
	totalRatios = totalRatios0;
	int denominator = 0;
	for (int i=0; i<totalRatios0; i++) {
		Rates[i].rate = rate0[i];
		
		denominator = 0;
		do
			denominator++;
		while (modNullApprox(denominator, 1 / rate0[i]) != 0);
			
		Rates[i].num = (int)(rate0[i] * denominator + 0.5);
		Rates[i].den = denominator;
	}
}

void Lfo::fillWT(FloatArray wt0) {  
	FloatArray tempArray;
	
	for (int WFid=0; WFid<WAVETABLE_SIZE; WFid++) {
		tempArray = wt0.subArray(WFid*WAVE_LENGTH, WAVE_LENGTH);
		for (int i=0; i<WAVE_LENGTH; i++) {
			wt[WFid][i] = tempArray.getElement(i) * 0.5 + 0.5;		// [0;1] bounds, center at 0.5
		}
	}
	FloatArray::destroy(tempArray);
}

float Lfo::getOutAtPhase(double phase0) {   
					
	int select = typeSel;
	double lfoPhase = phase0 + 1.0;
	lfoPhase = lfoPhase - (double)((int)lfoPhase);
	
	//if (lfoPhase >= 1) {
		//lfoPhase -= 1;
	//}
	//if (lfoPhase <0) {
		//lfoPhase += 1;
	//}
	
	switch(select) {
		
		case	0:	// downRamp
		
			lfo = 1 - lfoPhase;
			
			break;
			
		case	1:	// upRamp
		
			lfo = lfoPhase;
			
			break;
			
		case	2: 	// triangle
		
			lfo = getTriLfo(lfoPhase);
			
			break;
			
		case	3: 	// sinus
		
			lfo = sinf(2*M_PI * lfoPhase) / 2 + 0.5;
			
			break;
			
		case	4:	// square
		
			lfo = (int)(1.5 - lfoPhase);
			
			break;
			
		case	5:	// externalWave
		
			lfo = getExtWave(lfoPhase);
		
			break;
			
		default: 	//upRamp	
		
			lfo = lfoPhase;
		
	}
	
	return lfo;
	
}
	
float Lfo::getTriLfo(double phase0) {   				
	
	switch((int)(phase0*4)) {
		
		case	0:
			lfo = 0.5 + phase0*2;
			break;
			
		case	1:
		case	2:
			lfo = 1.5 - phase0*2;
			break;
			
		case	3:
			lfo = phase0*2 - 1.5;
			break;
			
		default:
			lfo = 1;
	}
	return lfo;
}
	
float Lfo::getExtWave(double phase0) {   				
	
	int intPart = phase0 * WAVE_LENGTH;
	double fracPart = phase0 * WAVE_LENGTH - intPart;
	lfo = (1.0-fracPart)*wt[waveSel][intPart] + fracPart*wt[waveSel][intPart+1];
	return lfo;
}

void Lfo::updatePhase() {  
	/*
	if (rate != ratem1) {
		phase = // que faire ici ??????
	}
	else {
		phase += bpm * getBlockSize() / getSampleRate() / 60 / rate;
	}*/
	//phase += (double)bpm * (double)blockSize / (double)60 / (double)sr / (double)Rates[rateSel].rate;
	
	phase += (double)bpm * (double)blockSize * (double)Rates[rateSel].den / (double)60 / (double)sr / (double)Rates[rateSel].num;
	
	if (phase >= 1) {
		phase -= 1;
	}
	
	ratem1 = Rates[rateSel].rate;
}
    
    
void Lfo::setClk(bool clockin) {  
	
if (clockin) {
	if (Rates[rateSel].rate <= 1 && Rates[rateSel].num == 1) {
		phase = 0;
	}
	else {
		int phaseNum = (int)(phase * Rates[rateSel].num + 0.5);

		if (Rates[rateSel].rate < 1) {
			phase = (double)phaseNum / (double)Rates[rateSel].den;

			if (phase >= 1) {
				phase = 0;			// or -= 1 ?
			}
		}
		else {
			phase = (double)phaseNum / (double)Rates[rateSel].num;

			if (phase >= 1) {
				phase = 0;
			}
		}
	}
}
}	    

float Lfo::modNullApprox(float numerator, float denominator) {
	float modulo = numerator/denominator;
	modulo -= (int)modulo;
	if (modulo > 0.9999 || modulo < 0.0001) {
		modulo = 0;
	}
	
	return modulo;
}

#endif   // __Lfo_cpp__
