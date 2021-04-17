#ifndef __MorphMagusPatch_hpp__
#define __MorphMagusPatch_hpp__

#include "MonochromeScreenPatch.h"
#include "Envelope.h"
#include "WTFactory.h"
#include "VoltsPerOctave.h"
#include "MonochromeAudioDisplay.hpp"
#include "ClkDetector.hpp"

#define baseFrequency (20)  /* starting frequency of first table */  // c1 = 32.7 Hz

#include "morphing/wavetables/spectral64.h"		/* left channel wavetable */ 
#include "morphing/wavetables/mikeS64.h"	/* right channel wavetable */ 

class MorphMagusPatch : public MonochromeScreenPatch {
	VoltsPerOctave hzL;
private:
  MorphOsc *morphL;
  MorphOsc *morphR;
  AdsrEnvelope *longEnv;
  AdsrEnvelope *shortEnv;
  ClkDetector *clkDet;
  
  SmoothFloat freqL;
  float FML;
  SmoothFloat freqR;
  float FMR;
  float morphXL;
  float morphYL;
  float morphXR;
  float morphYR;
  
  float threshold;
  float BPM;	
  float phase;	
	
  int debug;
  
public:
  MorphMagusPatch() {	
	  					
	debug = 0;
	  																													
	FloatArray bankL(spectral64[0], SAMPLE_LEN*NOF_Y_WF*NOF_X_WF);																																
	//FloatArray bankR(mikeS64[0], SAMPLE_LEN*NOF_Y_WF*NOF_X_WF);
	WTFactory *wtf = new WTFactory();

	morphL = new MorphOsc();
	morphR = new MorphOsc();
	wtf->makeMatrix(morphL, bankL, baseFrequency);
	//bankL.destroy(bankL);																															
	//FloatArray bankR(mikeS64[0], SAMPLE_LEN*NOF_Y_WF*NOF_X_WF);
	wtf->makeMatrix(morphR, bankL, baseFrequency);				// has to take bankR for L/R different wavetables
	   
	   
    clkDet = new ClkDetector(getSampleRate(), getBlockSize());
    threshold = 0.07
    BPM = 240;
    phase = 0;
    
	float defaultA = 0.00000001; 		// short time not applying
	float defaultD = 0.0000001;			// TO DO : short envelope
	float defaultR = 0.000000002;
	
	shortEnv = new AdsrEnvelope(getSampleRate());	
    shortEnv->setAttack(defaultA);
    shortEnv->setDecay(defaultD);
    shortEnv->setSustain(0.697f);
    shortEnv->setRelease(defaultR);
	
	longEnv = new AdsrEnvelope(getSampleRate());	
    longEnv->setAttack(defaultA*7);
    longEnv->setDecay(defaultD*6);
    longEnv->setSustain(0.9223f);
    longEnv->setRelease(defaultR*10);
    
	
// Morhp oscilator Left
	  registerParameter(PARAMETER_A, "TUNE");
	  registerParameter(PARAMETER_B, "FM L");
	  registerParameter(PARAMETER_AA, "MORPH X L");
	  registerParameter(PARAMETER_AB, "MORPH Y L");

    setParameterValue(PARAMETER_A, 0.5);
    setParameterValue(PARAMETER_B, 0.5);
    //setParameterValue(PARAMETER_AA, 70.0/8.0);	// to test max CPU
    //setParameterValue(PARAMETER_AB, 70.0/8.0);
    setParameterValue(PARAMETER_AA, 0.0);
    setParameterValue(PARAMETER_AB, 0.0);
	  
// Morhp oscilator Right
	  registerParameter(PARAMETER_C, "OFFSET R");
	  registerParameter(PARAMETER_D, "FM R");
	  registerParameter(PARAMETER_AC, "MOPRH X R");
	  registerParameter(PARAMETER_AD, "MORPH Y R");

    setParameterValue(PARAMETER_C, 0.01);
    //setParameterValue(PARAMETER_C, 1.0);			// to test max CPU
    setParameterValue(PARAMETER_D, 0.5);
    setParameterValue(PARAMETER_AC, 0.0);		
    setParameterValue(PARAMETER_AD, 0.0);
    //setParameterValue(PARAMETER_AC, 70.0/8.0);		// to test max CPU
    //setParameterValue(PARAMETER_AD, 70.0/8.0);		
    
// Modulations
    registerParameter(PARAMETER_E, "LFO ATTEN");
    registerParameter(PARAMETER_F, "LFO RATE");		
    registerParameter(PARAMETER_AE, "LFO1 SLOW>");
    registerParameter(PARAMETER_AF, "LFO2 FAST>");
    
    setParameterValue(PARAMETER_E, 0.84);
    setParameterValue(PARAMETER_F, 0.25);    
    setParameterValue(PARAMETER_AE, 0.0);
    setParameterValue(PARAMETER_AF, 0.0);
    
// Modulations
    registerParameter(PARAMETER_G, "ENV1 ATTEN");
    registerParameter(PARAMETER_H, "ENV2 S");
    registerParameter(PARAMETER_AG, "ENV1 LONG>");
    registerParameter(PARAMETER_AH, "ENV2 SHORT>");
    
    setParameterValue(PARAMETER_G, 0.41);
    setParameterValue(PARAMETER_H, 0.697);    
    setParameterValue(PARAMETER_AG, 0.0);
    setParameterValue(PARAMETER_AH, 0.0);
    
// Top parameters
	registerParameter(PARAMETER_BA, "GATE1"); 
    registerParameter(PARAMETER_BB, "GATE2");
    registerParameter(PARAMETER_BC, "VCA");
    registerParameter(PARAMETER_BD, "TEMPO");
  
    setParameterValue(PARAMETER_BA, 0.0); 
    setParameterValue(PARAMETER_BB, 0.0);
    setParameterValue(PARAMETER_BC, 0.0);
    setParameterValue(PARAMETER_BD, 0.0);
  
;  }
~MorphMagusPatch(){
  }
  
  void processAudio(AudioBuffer &buffer) {
    
    float gain = getParameterValue(PARAMETER_BC);
    float npast;
    float FML = getParameterValue(PARAMETER_B);
    float FMR = getParameterValue(PARAMETER_D);
    float tune = getParameterValue(PARAMETER_A)-0.5;
    //float tune = getParameterValue(PARAMETER_BA)-0.5;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    hzL.setTune((tune/12.0)-4.0);
    float freq = hzL.getFrequency(left[0]);
    freqL = exp(log(freq)+log(2));
    morphXL = getParameterValue(PARAMETER_AA);  
    morphYL = getParameterValue(PARAMETER_AB); 
    
	float offsetR = getParameterValue(PARAMETER_C)*12*2;
	//offsetR = (int) offsetR;
    freqR = exp(log(freq)+log(2)*offsetR/12.0);
    morphXR = getParameterValue(PARAMETER_AC);  
    morphYR = getParameterValue(PARAMETER_AD); 
    
    
    morphL->setMorphY(morphYL);		
    morphL->setMorphX(morphXL);
    morphR->setMorphY(morphYR);
    morphR->setMorphX(morphXR);
    
	
	debug = clkDet->setCLK(getParameterValue(PARAMETER_BD), threshold);
	BPM = (clkDet->getBPM());
	if (clkDet->isPhase0())
	{
		phase = 0; // this is forced phase, phase has to be looped somewhere else
	}
    
    float amount = (getParameterValue(PARAMETER_E)-0.5)*2; 			// attenuverter
    int rate = pow(2, (int) ((getParameterValue(PARAMETER_F)*4)));
    
    static int ratem1 = rate;
    
    static float lfo1 = 0;
    lfo1 += BPM * getBlockSize() / getSampleRate() / 60;
    if(lfo1 > 1.0 || phase == 0){
      lfo1 = 0;
    }
    else{
    }
    float lfoInv = lfo1*amount;
    if (lfoInv < 0) {
		lfoInv += 1*(-amount);
	}
    setParameterValue(PARAMETER_AE, lfoInv);	
    
    if (ratem1 != rate)  {
		phase = 2*M_PI * (lfo1 - rate * BPM * getBlockSize() / getSampleRate() / 60);		
	}																					// TO DO : wait for the phase to sync when switching
    //amount = 0.3;
        phase += 2*M_PI * rate * BPM * getBlockSize() / getSampleRate() / 60;
        if(phase >= 2*M_PI)
          phase -= 2*M_PI;
    setParameterValue(PARAMETER_AF, sinf(phase)/3*amount+0.5); 		// +0.5 centered for attenuation
    	
    
    ratem1 = rate;
    
    
    if(getParameterValue(PARAMETER_BA) > threshold){
      longEnv->gate(true);
    }
    else longEnv->gate(false);
    amount = getParameterValue(PARAMETER_G); 			// TO DO : negative envelope
    setParameterValue(PARAMETER_AG, longEnv->getNextSample()*amount);
    
    shortEnv->setSustain(getParameterValue(PARAMETER_H));
    if(getParameterValue(PARAMETER_BB) > threshold){
      shortEnv->gate(true);
    }
    else shortEnv->gate(false);
    setParameterValue(PARAMETER_AH, shortEnv->getNextSample());
    
    for(int n = 0; n<buffer.getSize(); n++){
		
    morphL->setFrequency(freqL+right[n]/gain*FML*freqL/2);
	morphR->setFrequency(freqR+right[n]/gain*FMR*freqR/2);
	left[n] = (morphL->getMorphOutput()) * gain;
	right[n] = morphR->getMorphOutput() * gain;	
    morphL->updatePhase();				
    morphR->updatePhase();
		
	}    
	
    display.update(left, 2, 0.0, 3.0, 0.0);
    //debugMessage("out" , (int)(rate), morphR->getInferiorIndex() )	;	
    debugMessage("out" , ((float)(debug)/((float)(fourSec))), BPM )	;	

	}
	
MonochromeAudioDisplay display;

  void processScreen(MonochromeScreenBuffer& screen){
    display.draw(screen, WHITE);
   }
};

#endif   // __MorphMagusPatch
