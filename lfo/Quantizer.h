#ifndef __Quantizer_h__
#define __Quantizer_h__

#include "Patch.h"
//#include "Scaler.h"

//const int numRateSlots = 81;

//typedef struct {
    //float rate;
    //int num;
    //int den;
//} ratio;

class Quantizer {
	
private:

	//Scaler *scaler;
	
	//unsigned int totalRatios;
	float out;
	float bpm;
	float shapeSel;
	float shapeStart;
	float scaleSel;
	float scaleKey;
	float holdValue;
	float goalValue;
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
	Quantizer(float sr0, int blockSize0, float scaleKey0);
	~Quantizer();
	
	
	float getOut();
	bool isQuantizing();
	float quantize(float in);
	void updatePhase();
	void setClk(bool clockin);
	float modNullApprox(float numerator, float denominator);
	void setKey(float keyFreq0);
	
	void reset();
	void setBpm(float bpm0);
	void setRate(float rate0);
	void selectShape(float shapeSel0);
	void setShapeStart(float shapeStart0);
	void selectScale(float scaleSel0);
	void hold(float in);
	void goal(float in);
	void offsetPhase(double phaseOffs0);
	//float getBpm();
	//float getRate();
	
};

	
//inline float Quantizer::getOut() {
	//return out;
//}

	
inline void Quantizer::reset() {
	phase = 0.0;
}
	
	
inline void Quantizer::setBpm(float bpm0) {
	bpm = bpm0;
}
	
	
inline void Quantizer::setRate(float rate0) {
	rate = rate0;
}	
	
	
inline void Quantizer::selectShape(float shapeSel0) {
	shapeSel = shapeSel0;
}	
	
	
inline void Quantizer::setShapeStart(float shapeStart0) {
	shapeStart = shapeStart0;
}	
	
	
inline void Quantizer::selectScale(float scaleSel0) {
	scaleSel = scaleSel0;
}
	
	
inline void Quantizer::hold(float in) {
	holdValue = in;
}	
	
	
inline void Quantizer::goal(float in) {
	goalValue = in;
}	
	
inline void Quantizer::offsetPhase(double phaseOffs0) {
	phaseOffs = phaseOffs0;
	if (phaseOffs < (-1.0) || phaseOffs > 1.0) {
		phaseOffs = phaseOffs - (int)phaseOffs;
	}
}


#endif // __Quantizer_h__
