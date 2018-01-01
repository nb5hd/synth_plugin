//
//  Oscillator.h
//  Synthesis
//
//  Created by Nikhil Bhaip on 12/29/17.
//

#ifndef Oscillator_h
#define Oscillator_h

#include <math.h>

// different types of waves
enum OscillatorMode{
    OSCILLATOR_MODE_SINE,
    OSCILLATOR_MODE_SAW,
    OSCILLATOR_MODE_SQUARE,
    OSCILLATOR_MODE_TRIANGLE
};

class Oscillator {
private:
    OscillatorMode mOscillatorMode;
    const double mPI;
    double mFrequency;
    double mPhase; // where in waveform cycle cycle currently is (always changing in time)
    double mSampleRate;
    double mPhaseIncrement; // amount that is added to phase each sample
    void updateIncrement();
public:
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void generate(double* buffer, int nFrames); // inputs buffer of doubles and fills in sample values
    Oscillator():
        mOscillatorMode(OSCILLATOR_MODE_SINE),
        mPI(2*acos(0.0)),
        mFrequency(440.0),
        mPhase(0.0),
        mSampleRate(44100.0){
            updateIncrement();
        };
};




#endif /* Oscillator_h */
