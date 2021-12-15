#ifndef __FalafLfoPatch_hpp__
#define __FalafLfoPatch_hpp__

#include "OpenWareLibrary.h"
#include "MonochromeScreenPatch.h"
#include "VoltsPerOctave.h"
//#include "MonochromeAudioDisplay.hpp"
#include "ClkDetector.h"
#include "Lfo.h"
#include "Quantizer.h"

#define WAVEFORM_SIZE 256
#define WAVETABLE_SIZE 64
 
 
unsigned const int totalRatios1 = 11;
unsigned const int totalRatios2 = 23;
double rate1[totalRatios1] = { 1.0/4.0, 1.0/3.0, 1.0/2.0, 2.0/3.0, 3.0/4.0, 1.0, 3.0/2.0, 2.0, 3.0, 4.0, 8.0};
double rate2[totalRatios2] = { 1.0/32.0, 1.0/16.0, 1.0/8.0, 1.0/7.0, 1.0/6.0, 1.0/5.0, 1.0/4.0, 1.0/3.0, 1.0/2.0, 2.0/3.0, 3.0/4.0, 1.0, 3.0/2.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 12.0, 16.0, 32.0 };
	

/* TO DO : 
 * 
 * use audio out as DC lfo 
 * LEFT OUT: LFO , RIGHT IN: QUANT IN , RIGHT OUT: QUANT OUT
 * 
 * debug :
 * 
 * center extwaves (and bound them?)
 * quant shape not working at all, only jitter: 
 * That probably is wrong goalValue update 
 * & also wrong shape calculation (none of them different types gives good result)
 * 
*/

class FalafLfoPatch : public MonochromeScreenPatch {
	VoltsPerOctave hzL;
private:
	
	FloatArray displayFloat;
	
	WhiteNoiseGenerator *noizz;
	
  Lfo *lfo1;
  Lfo *lfo2;
  Quantizer *quantizer;
  ClkDetector *clkDet;
	
	FloatArray createWavebank(const char* name){
		Resource* resource = getResource(name);
		WavFile wav(resource->getData(), resource->getSize());
		if(!wav.isValid())
			error(CONFIGURATION_ERROR_STATUS, "Invalid wav");
		FloatArray bank = wav.createFloatArray(0);
		Resource::destroy(resource);
		return bank;
	}
  
  float threshold;
  float bpm;	
  //float phase;
  float nextPhase1;
  float nextPhase2;
  int totalLfo1Types;
  int totalLfo2Types;
  float temp;
	
  int debugInt;
  float debugFloat;
  
public:
  FalafLfoPatch() {	
	  
	displayFloat = FloatArray::create(getBlockSize());
	displayFloat.clear();
	debugInt = 0;
	debugFloat = 0;
	temp = 0;
	
    // hzL.setTune(-4.0);
    hzL.setTune(0.0);
	
	noizz = WhiteNoiseGenerator::create();
	
	FloatArray wt1 = createWavebank("wavetable1.wav");
    FloatArray wt2 = createWavebank("wavetable2.wav"); 	
	
	lfo1 = new Lfo(getSampleRate(), getBlockSize());
	lfo1->fillRates(rate1, totalRatios1);
	lfo1->fillWT(wt1);	
	
	lfo2 = new Lfo(getSampleRate(), getBlockSize());
	lfo2->fillRates(rate2, totalRatios2);
	lfo2->fillWT(wt2);
	
	quantizer = new Quantizer(getSampleRate(), getBlockSize(), hzL.getFrequency(0.0));
																																
	
	clkDet = new ClkDetector(getSampleRate(), getBlockSize());
	
    threshold = 0.07;
    bpm = 120;
    
    totalLfo1Types = lfo1->getTotalTypes();
    totalLfo2Types = lfo2->getTotalTypes();
    
    FloatArray::destroy(wt1);
    FloatArray::destroy(wt2);
	
// LFO1 Sync to MTEMPO
    registerParameter(PARAMETER_A, "LFO1 RATE");
    registerParameter(PARAMETER_B, "LFO1 TYPE");		
    registerParameter(PARAMETER_AA, "LFO1 OFFS");
    registerParameter(PARAMETER_AB, "LFO1>");
    
    setParameterValue(PARAMETER_A, 0.51);
    setParameterValue(PARAMETER_B, 0.5);    
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.0);	
	  
// LFO2 sync to MTEMPO
    registerParameter(PARAMETER_C, "LFO2 RATE");		
    registerParameter(PARAMETER_D, "LFO2 TYPE");		// random mode : OFFS becomes MIX between LFO1 and random noise
    registerParameter(PARAMETER_AC, "LFO2 OFFS");		// last type being random mode
    registerParameter(PARAMETER_AD, "LFO2>");
    
    setParameterValue(PARAMETER_C, 0.51);
    setParameterValue(PARAMETER_D, 0.5);    
    setParameterValue(PARAMETER_AC, 0.0);
    setParameterValue(PARAMETER_AD, 0.0);
    
// MIX
    registerParameter(PARAMETER_E, "MIXER");		
    registerParameter(PARAMETER_F, "ATTENUV");		
    registerParameter(PARAMETER_AE, "WAVE SEL");
    registerParameter(PARAMETER_AF, "LFO MIX>");	// this same signal is sent to quantizer
    
    setParameterValue(PARAMETER_E, 0.5); 
    setParameterValue(PARAMETER_F, 1.0);   
    setParameterValue(PARAMETER_AE, 0.0);	
    setParameterValue(PARAMETER_AF, 0.0); 	
    
// QUANTIZER sync to MTEMPO
    registerParameter(PARAMETER_G, "Q RATE");		// TO DO : find the most convenient
    registerParameter(PARAMETER_H, "Q SHAPE");		// TO DO : no quantize
    registerParameter(PARAMETER_AG, "Q OFFS");
    registerParameter(PARAMETER_AH, "QUANT OUT>");
    
    setParameterValue(PARAMETER_G, 1.0);
    setParameterValue(PARAMETER_H, 0.0);   
    setParameterValue(PARAMETER_AG, 0.5);
    setParameterValue(PARAMETER_AH, 0.0);
    
// Top parameters
	registerParameter(PARAMETER_BA, "TEMPO"); 
    registerParameter(PARAMETER_BB, "RESET");		
    registerParameter(PARAMETER_BC, "SHAPE START");	
    registerParameter(PARAMETER_BD, "Q SCALE");
  
    setParameterValue(PARAMETER_BA, 0.0); 
    setParameterValue(PARAMETER_BB, 0.0);
    setParameterValue(PARAMETER_BC, 1.0);
    setParameterValue(PARAMETER_BD, 0.0);
    
    
  
;  }
~FalafLfoPatch(){
	WhiteNoiseGenerator::destroy(noizz);
	delete clkDet;
	delete lfo1;
	delete quantizer;
    FloatArray::destroy(displayFloat);
  }

  
  void processAudio(AudioBuffer &buffer) {
	  
	  
	debugInt = bpm;
	//debugFloat = lfo1->getRate();
	
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    
    
    float noise = noizz->generate() + 0.5;
    
    //// Clocks
    ////
	
	clkDet->setClk(getParameterValue(PARAMETER_BA), threshold);
	if (clkDet->isModeSwitchin() || clkDet->isReset()) {
		lfo1->reset();
		lfo2->reset();
		quantizer->reset();
	}
	bpm = (clkDet->getBpm());
    
    
    //// Modulations
    ////
    
    
    //// LFOs 
    
    float out1;
	double lfo1PhaseOffset = getParameterValue(PARAMETER_AA) - 0.5;
	// lfo1PhaseOffset = (-lfo1PhaseOffset);					// Inverse offset depending on your preference
	double lfo1Rate = rate1[(int)(getParameterValue(PARAMETER_A)*totalRatios1)];
	lfo1->selectExtWave(getParameterValue(PARAMETER_AE)*WAVETABLE_SIZE);
	lfo1->selectRate(getParameterValue(PARAMETER_A));
	lfo1->setBpm(bpm);
    
	if (clkDet->isClockin())
	{
		lfo1->setClk(true);			// recalculate the phase
	}
	
	lfo1->selectLfoType(getParameterValue(PARAMETER_B) * (totalLfo1Types + 1) / totalLfo1Types);
	// lfo1->offsetPhase(lfo1PhaseOffset);
	if(getParameterValue(PARAMETER_B) * (totalLfo1Types + 1) < totalLfo1Types) {	// add one type for noise gen
		out1 = lfo1->getOutAtPhase(lfo1->getPhase() + lfo1PhaseOffset);
	}
	else {
		out1 = noise;
	}
	setParameterValue(PARAMETER_AB, out1);
    
    
    float out2;
	double lfo2PhaseOffset = getParameterValue(PARAMETER_AC) - 0.5;
	// lfo2PhaseOffset = (-lfo2PhaseOffset);					// Inverse offset depending on your preference
	double lfo2Rate = rate2[(int)((getParameterValue(PARAMETER_C) * totalRatios2))];
	lfo2->selectExtWave(getParameterValue(PARAMETER_AE) * WAVETABLE_SIZE);
	lfo2->selectRate(getParameterValue(PARAMETER_C));
	lfo2->setBpm(bpm);
    
	if (clkDet->isClockin())
	{
		lfo2->setClk(true);			// recalculate the phase
	}
	
	lfo2->selectLfoType(getParameterValue(PARAMETER_D) * (totalLfo2Types + 1) / totalLfo2Types);
	// lfo1->offsetPhase(lfo1PhaseOffset);
	if(getParameterValue(PARAMETER_D) * (totalLfo2Types + 1) < totalLfo2Types) {	// add one type for noise gen
		out2 = lfo2->getOutAtPhase(lfo2->getPhase() + lfo2PhaseOffset);
	}
	else {
		out2 = noise;
	}
	setParameterValue(PARAMETER_AD, out2);
    
    //// Mixer
	
	float mixer = getParameterValue(PARAMETER_E);
	float mix;
	mix = (out1 * (1 - mixer) + out2 * mixer);
	mix = attenuvert(mix, getParameterValue(PARAMETER_F), 0.5);
	setParameterValue(PARAMETER_AF, mix);
    
    //// Quantizer
    
    double quantRate;									// TODO: power of 2 divisions
    if(getParameterValue(PARAMETER_G) < 0.499999) {		// divide lfo1 rate up to 16
		quantRate = (double)lfo1Rate/(double)((int)((0.5 - getParameterValue(PARAMETER_G)) * 32.0 + 0.999999));
		}
	else if(getParameterValue(PARAMETER_G) < 0.5) {
		quantRate = (double)lfo1Rate;
		}
	else if(getParameterValue(PARAMETER_G) < 0.500001) {
		quantRate = (double)lfo2Rate;
		}
	else {										// when param > 0.5 , divide lfo2 rate up to 16
		quantRate = (double)lfo2Rate/(double)((int)((getParameterValue(PARAMETER_G) - 0.5) * 32.0 + 0.999999));
		}
			 
    //double quantRate = rate1[getParameterValue(PARAMETER_A)*totalRatios1]/pow(2, (int) ((getParameterValue(PARAMETER_G)*6)));
    double quantBpm = bpm;
    double quantPhaseOffset = getParameterValue(PARAMETER_AG) - 0.5;
    quantizer->setRate(quantRate);
	quantizer->setBpm(quantBpm);
	quantizer->selectShape(getParameterValue(PARAMETER_H));
	quantizer->setShapeStart(getParameterValue(PARAMETER_BC));
	quantizer->selectScale(getParameterValue(PARAMETER_BD));
	quantizer->setKey(hzL.getFrequency(left[0]));
    
	if (clkDet->isClockin())
	{
		quantizer->setClk(true);	// recalculate the phase
	}
    
	quantizer->offsetPhase(quantPhaseOffset);
		
	if (quantizer->isQuantizing()) {
		
		quantizer->hold(mix);
		//debugFloat = mix - temp;
		debugFloat = lfo1->getPhase() + lfo1PhaseOffset + 1.0;
		debugFloat -= (int)(lfo1->getPhase() + lfo1PhaseOffset + 1.0);
		debugFloat -= nextPhase1;
		
		
		nextPhase1 = quantRate / rate1[(int)(getParameterValue(PARAMETER_A)*totalRatios1)] + lfo1->getPhase() + lfo1PhaseOffset + 1.0;
		nextPhase1 = (double)(nextPhase1 - (double)((int) nextPhase1));
		nextPhase2 = quantRate / rate2[(int)(getParameterValue(PARAMETER_C)*totalRatios2)] + lfo2->getPhase() + lfo2PhaseOffset + 1.0;
		nextPhase2 = (double)(nextPhase2 - (double)((int) nextPhase2));
		
	noise = noizz->generate() + 0.5;
	if(getParameterValue(PARAMETER_B) * (totalLfo1Types + 1) < totalLfo1Types) {
		mix = lfo1->getOutAtPhase(nextPhase1) * (1 - mixer);
	}
	else {
		mix = noise * (1 - mixer);
	}
	if(getParameterValue(PARAMETER_D) * (totalLfo2Types + 1) < totalLfo2Types) {
		mix += lfo2->getOutAtPhase(nextPhase2) * mixer;
	}
	else {
		mix += noise * mixer;
	}
	
	mix = attenuvert(mix, getParameterValue(PARAMETER_F), 0.5);
	quantizer->goal(mix);
	temp = mix;
		//debugFloat -= mix;
	}
	
	setParameterValue(PARAMETER_AH, quantizer->getOut());
	
    
    //// Audio block
    ////
    
    //for(int n = 0; n<buffer.getSize(); n++){

		
	//}    
	
	
	//// Message display & Debug
	////
	
	
    //debugMessage("out" , (int)(rate), morphR->getInferiorIndex() )	;	
    debugMessage("out" , ((float)(debugInt)), debugFloat)	;	
	
	lfo1->updatePhase();
	lfo2->updatePhase();
	quantizer->updatePhase();

	}
	
	//// Screen
	////
	
	//MonochromeAudioDisplay display;
	
    //display.update(displayFloat, 2, 0.0, 3.0, 0.0);
	void processScreen(MonochromeScreenBuffer& screen){
    //display.draw(screen, WHITE);
	}
   
	float attenuvert(float in, float param, float center) {
		float out = (in - center) * (param - center) * 2.0 + center;
		return out;
	}
   
};

#endif   // __FalafLfoPatch
