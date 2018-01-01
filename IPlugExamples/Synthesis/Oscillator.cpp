//
//  Oscillator.cpp
//  Synthesis
//
//  Created by Nikhil Bhaip on 12/29/17.
//

#include <Oscillator.h>

void Oscillator::setMode(OscillatorMode mode){
    mOscillatorMode = mode;
}

void Oscillator::setFrequency(double frequency){
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate){
    mSampleRate = sampleRate;
    updateIncrement();
}

// Explanation for formula: http://basicsynth.com/index.php?page=basic
void Oscillator::updateIncrement(){
    // units for phase increment are: cycles / sample
    mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
}

void Oscillator::generate(double *buffer, int nFrames){
    const double twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            // We just increment mPhase and ensure it's between 0 and 2*Pi
            for(int i=0; i < nFrames; i++){
                buffer[i] = sin(mPhase);
                mPhase += mPhaseIncrement;
                while(mPhase >= twoPI){
                    mPhase -= twoPI;
                }
            }
            break;
            
        case OSCILLATOR_MODE_SAW: // downwards saw wave
            for(int i=0; i < nFrames; i++){
                buffer[i] = 1.0 - (2.0 *mPhase / twoPI); // range is [-1, 1]
                mPhase += mPhaseIncrement;
                while(mPhase >= twoPI){
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for(int i=0; i < nFrames; i++){
                if(mPhase <= mPI){
                    buffer[i]=1.0;
                } else {
                    buffer[i]=-1.0;
                }
                mPhase += mPhaseIncrement;
                while(mPhase >= twoPI){
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            for(int i=0; i < nFrames; i++){
                // saw_val is actually upwards saw wave (note opposite signs)
                double saw_val = -1.0 + (2.0 *mPhase / twoPI); // range is [-1, 1]
                buffer[i] = 2.0 * (fabs(saw_val) - 0.5); // converts range from [-1,1] to [0,1]
                mPhase += mPhaseIncrement;
                while(mPhase >= twoPI){
                    mPhase -= twoPI;
                }
            }
            break;
            
        default:
            break;
    }
}

