#ifndef __SYNTHESIS__
#define __SYNTHESIS__

#include "IPlug_include_in_plug_hdr.h"
#include "Oscillator.h"

class Synthesis : public IPlug
{
public:
  Synthesis(IPlugInstanceInfo instanceInfo);
  ~Synthesis();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void UpdateFrequencyText(ITextControl* showSampleFreq);

private:
  double mFrequency;
  int mOscillatorMode;
  void CreatePresets();
  Oscillator mOscillator;
  ITextControl* showSampleFreq;
};

#endif
