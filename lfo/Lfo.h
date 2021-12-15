#ifndef __Lfo_h
#define __Lfo_h

#include "Patch.h"

#define WAVE_LENGTH 256
#define WAVETABLE_SIZE 64

const int numRateSlots = 81;
const int totalLfoTypes = 6;

typedef struct {
    double rate;
    int num;
    int den;
} ratio;

class Lfo {
	
private:

	unsigned int totalRatios;
	float lfo;
	float bpm;
	double phase;
	double phaseOffs;
	int rateSel;
	int typeSel;
	int waveSel;
	float wt[WAVETABLE_SIZE][WAVE_LENGTH];
	float ratem1;
	float sr;
	int blockSize;
	
public:
	
	Lfo();
	Lfo(float sr0, int blockSize0);
	~Lfo();
	
	ratio Rates[numRateSlots];
	
	void fillRates(double* rate0, int totalRatios0);
	void fillWT(FloatArray wt0);
	float getOutAtPhase(double phase0);
	float getTriLfo(double phase0);
	float getExtWave(double phase0);
	void updatePhase();
	void setClk(bool clockin);
	float modNullApprox(float numerator, float denominator);
	
	void reset();
	void setBpm(float bpm0);
	void selectRate(float rateSel0);
	void selectLfoType(float typeSel0);
	void selectExtWave(float extWaveSel0);
	void offsetPhase(double phaseOffs0);
	float getBpm();
	float getRate();
	double getPhase();
	int getTotalTypes();
	
};

	
inline void Lfo::reset() {
	phase = 0.0;
}
	
	
inline void Lfo::setBpm(float bpm0) {
	bpm = bpm0;
}
	
	
inline void Lfo::selectRate(float rateSel0) {
	rateSel = rateSel0 * totalRatios;
}	
	
inline void Lfo::selectLfoType(float typeSel0) {
	typeSel = typeSel0 * totalLfoTypes;
}	
	
inline void Lfo::selectExtWave(float extWaveSel0) {
	waveSel = extWaveSel0;
}	
	
inline void Lfo::offsetPhase(double phaseOffs0) {
	phaseOffs = phaseOffs0;
	if (phaseOffs < (-1.0) || phaseOffs > 1.0) {
		phaseOffs = phaseOffs - (int)phaseOffs;
	}
}
	
	
inline float Lfo::getBpm() {
	return bpm;
}

inline float Lfo::getRate() {
	//return Rates[rateSel].rate;
	return phaseOffs;
}	
	
inline double Lfo::getPhase() {
	return phase;
}
	
inline int Lfo::getTotalTypes() {
	return totalLfoTypes;
}


#endif // __Lfo_h__
