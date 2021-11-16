#ifndef __Quantizer_h
#define __Quantizer_h

//#include "Patch.h"

//const int numRateSlots = 81;
//const int totalLfoForms = 6;

//typedef struct {
    //float rate;
    //int num;
    //int den;
//} ratio;

class Quantizer {
	
private:

	//unsigned int totalRatios;
	float out;
	float bpm;
	double phase;
	double phasem1;
	double phaseOffs;
	double rate;
	double ratem1;
	unsigned int rateNum;
	unsigned int rateDen;
	
	float sr;
	int blockSize;
	
public:
	
	Quantizer();
	Quantizer(float sr0, int blockSize0);
	~Quantizer();
	
	float quantize(float in);
	void updatePhase();
	void setClk(bool clockin);
	float modNullApprox(float numerator, float denominator);
	
	float getOut();
	void reset();
	void setBpm(float bpm0);
	void setRate(float rate0);
	void offsetPhase(double phaseOffs0);
	//float getBpm();
	//float getRate();
	
};

	
inline float Quantizer::getOut() {
	return out;
}

	
inline void Quantizer::reset() {
	phase = 0.0;
}
	
	
inline void Quantizer::setBpm(float bpm0) {
	bpm = bpm0;
}
	
	
inline void Quantizer::setRate(float rate0) {
	rate = rate0;
}	
	
inline void Quantizer::offsetPhase(double phaseOffs0) {
	phaseOffs = phaseOffs0;
	if (phaseOffs < (-1.0) || phaseOffs > 1.0) {
		phaseOffs = phaseOffs - (int)phaseOffs;
	}
}


#endif // __Quantizer_h__
