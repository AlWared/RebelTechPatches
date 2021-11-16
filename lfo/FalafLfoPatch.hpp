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
 * use wavetables for lfo
*/

class FalafLfoPatch : public MonochromeScreenPatch {
	VoltsPerOctave hzL;
private:
	
	FloatArray displayFloat;
	
  Lfo *lfo1;
  // Lfo *lfo2;
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
	
  int debugInt;
  float debugFloat;
  
public:
  FalafLfoPatch() {	
	  
	displayFloat = FloatArray::create(getBlockSize());
	displayFloat.clear();
	debugInt = 0;
	debugFloat = 0;
	
	FloatArray wt1 = createWavebank("wavetable1.wav");
    FloatArray wt2 = createWavebank("wavetable2.wav"); 	
	
  //unsigned const int totalRatios1 = 11;
  //unsigned const int totalRatios2 = 23;
  //double rate1[totalRatios1] = { 1.0/4.0, 1.0/3.0, 1.0/2.0, 2.0/3.0, 3.0/4.0, 1.0, 3.0/2.0, 2.0, 3.0, 4.0, 8.0};
  //double rate2[totalRatios2] = { 1/32, 1/16, 1/8, 1/7, 1/6, 1/5, 1/4, 1/3, 1/2, 2/3, 3/4, 1, 3/2, 2, 3, 4, 5, 6, 7, 8, 12, 16, 32 };
	lfo1 = new Lfo(getSampleRate(), getBlockSize());
	lfo1->fillRates(rate1, totalRatios1);
	lfo1->fillWT(wt1);
	
	quantizer = new Quantizer(getSampleRate(), getBlockSize());
																																
	
	clkDet = new ClkDetector(getSampleRate(), getBlockSize());
    threshold = 0.07;
    bpm = 120;
    
    FloatArray::destroy(wt1);
    FloatArray::destroy(wt2);
	
// LFO1 Sync to MTEMPO
    registerParameter(PARAMETER_A, "LFO1 RATE");
    registerParameter(PARAMETER_B, "LFO1 OFFS");		
    registerParameter(PARAMETER_AA, "LFO1 TYPE");
    registerParameter(PARAMETER_AB, "LFO1>");
    
    setParameterValue(PARAMETER_A, 0.51);
    setParameterValue(PARAMETER_B, 0.5);    
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.0);	
	  
// LFO2 sync to MTEMPO
    registerParameter(PARAMETER_C, "LFO2 RATE");
    registerParameter(PARAMETER_D, "LFO2 OFFS");		
    registerParameter(PARAMETER_AC, "LFO2 TYPE");
    registerParameter(PARAMETER_AD, "LFO2>");
    
    setParameterValue(PARAMETER_C, 0.51);
    setParameterValue(PARAMETER_D, 0.5);    
    setParameterValue(PARAMETER_AC, 0.0);
    setParameterValue(PARAMETER_AD, 0.0);
    
// LFO3/JITTER initially sync to TEMPO BIS
    registerParameter(PARAMETER_E, "LFO3 TYPE");		// last type being jitter
    registerParameter(PARAMETER_AE, "LFO3>");
    
    setParameterValue(PARAMETER_E, 0.51); 
    setParameterValue(PARAMETER_AE, 0.0);		
    
// QUANTIZER sync to MTEMPO
    registerParameter(PARAMETER_F, "QUANT SCALE");		// TO DO : no quantize
    registerParameter(PARAMETER_G, "QUANT RATE");		// TO DO : find the most convenient
    registerParameter(PARAMETER_H, "ATTENUV");
    registerParameter(PARAMETER_AF, "QUANT OFFS");
    registerParameter(PARAMETER_AG, "QUANT IN");
    registerParameter(PARAMETER_AH, "QUANT OUT>");
    
    setParameterValue(PARAMETER_F, 0.0);   
    setParameterValue(PARAMETER_G, 1.0);
    setParameterValue(PARAMETER_H, 0.5);  
    setParameterValue(PARAMETER_AF, 0.5);  
    setParameterValue(PARAMETER_AG, 0.0);
    setParameterValue(PARAMETER_AH, 0.0);
    
// Top parameters
	registerParameter(PARAMETER_BA, "MTEMPO"); 
    registerParameter(PARAMETER_BB, "TEMPO BIS");		// when set to 1 for a while, it gets sync to MTEMPO
    registerParameter(PARAMETER_BC, "LEFT O SRC");
    registerParameter(PARAMETER_BD, "");
  
    setParameterValue(PARAMETER_BA, 0.0); 
    setParameterValue(PARAMETER_BB, 0.0);
    setParameterValue(PARAMETER_BC, 0.0);
    setParameterValue(PARAMETER_BD, 0.0);
    
    
  
;  }
~FalafLfoPatch(){
	delete clkDet;
	delete lfo1;
	delete quantizer;
    FloatArray::destroy(displayFloat);
  }

  
  void processAudio(AudioBuffer &buffer) {
    
    
    //// Clocks
    ////
	
	clkDet->setClk(getParameterValue(PARAMETER_BA), threshold);
	if (clkDet->isModeSwitchin() || clkDet->isReset()) {
		lfo1->reset();
		quantizer->reset();
	}
	bpm = (clkDet->getBpm());
    
    
    //// Modulations
    ////
    
    
    //// LFOs 
    
	double phaseOffset = getParameterValue(PARAMETER_B) - 0.5;
	// phaseOffset = (-phaseOffset);					// Inverse offset depending on your preference
	lfo1->selectExtWave(getParameterValue(PARAMETER_D)*WAVETABLE_SIZE);
	lfo1->selectRate(getParameterValue(PARAMETER_A));
	lfo1->setBpm(bpm);
    
	if (clkDet->isClockin())
	{
		lfo1->setClk(true);			// recalculate the phase
	}
	
	lfo1->selectLfoType(getParameterValue(PARAMETER_AA));
	lfo1->offsetPhase(phaseOffset);
	setParameterValue(PARAMETER_AB,lfo1->getLfo());
	lfo1->updatePhase();
    
    //// Quantizer
    
    double quantRate = (double)rate1[(int)(getParameterValue(PARAMETER_A)*totalRatios1)]/(double)((int) ((1.0 - getParameterValue(PARAMETER_G)*16)));
    //double quantRate = rate1[getParameterValue(PARAMETER_A)*totalRatios1]/pow(2, (int) ((getParameterValue(PARAMETER_G)*6)));
    phaseOffset = getParameterValue(PARAMETER_H) - 0.5;
    quantizer->setRate(quantRate);
	quantizer->setBpm(bpm);
    
	if (clkDet->isClockin())
	{
		quantizer->setClk(true);	// recalculate the phase
	}
    
	quantizer->offsetPhase(phaseOffset);
	quantizer->quantize(getParameterValue(PARAMETER_AG));
	setParameterValue(PARAMETER_AH,quantizer->getOut());
	quantizer->updatePhase();
	

    
    //// Audio block
    ////
    
    //for(int n = 0; n<buffer.getSize(); n++){

		
	//}    
	
	
	//// Message display & Debug
	////
	
	debugInt = bpm;
	debugFloat = lfo1->getRate();
	
    //debugMessage("out" , (int)(rate), morphR->getInferiorIndex() )	;	
    debugMessage("out" , ((float)(debugInt)), debugFloat)	;	

	}
	
	//// Screen
	////
	
	//MonochromeAudioDisplay display;
	
    //display.update(displayFloat, 2, 0.0, 3.0, 0.0);
	//void processScreen(MonochromeScreenBuffer& screen){
    //display.draw(screen, WHITE);
	//}
   
};

#endif   // __FalafLfoPatch
