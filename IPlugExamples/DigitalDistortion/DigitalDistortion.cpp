#include "DigitalDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 5; // we have 5 presets/programs now

enum EParams
{
  kThreshold = 0,
  kNumParams // number of options = 1
  //Note that kNumParams has no initalizer
  //Why? "If the first enumerator does not have an initializer, the associated value is zero. For any other enumerator whose definition does not have an initializer, the associated value is the value of the previous enumerator plus one." - cppreference
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  // location of Threshold knob on plugin
  kThresholdX = 79,
  kThresholdY = 62,
  kKnobFrames = 128
};

DigitalDistortion::DigitalDistortion(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mThreshold(1.)
{
  TRACE;
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Threshold", 0., 0., 99.99, 0.01, "%");
  // shape determines starting point of knob
  // 1 means 12 o'clock pos
  GetParam(kThreshold)->SetShape(1.);
  //GetParam(kMode)->SetDisplayText(0, "a");
  
  // create the panel
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_RED);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);
  
  // load the knob sprite and add it to panel
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  
  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));
  
  AttachGraphics(pGraphics);
  
  CreatePresets();
}

DigitalDistortion::~DigitalDistortion() {}

// process a block of audio

void DigitalDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  int const numChannels = 2; // left and right channels
  
  for(int i=0; i < numChannels; i++){
    // note that i=0 means left and i=1 means right channel
    double* input = inputs[i];
    double* output = outputs[i];
    
    // nFrames = # of samples per channel so we know how long the buffers are.
    for(int s=0; s < nFrames; ++s, ++input, ++output){
      if(*input > 0){
        // ensures positive input values cannot go above our threshold
        *output = fmin(mThreshold, *input);
      } else {
        // ensures negative input values cannot go below our negative threshold
        *output = fmax(-mThreshold, *input);
      }
      *output /= mThreshold;
    }
  }
}

void DigitalDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}
// handle parameter changes
void DigitalDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case kThreshold:
      mThreshold = 1- (GetParam(kThreshold)->Value() / 100.);
      // debug message (when we change mThreshold value)
      DBGMSG( "value of Threshold : %f\n", mThreshold);
      break;
      
    default:
      break;
  }
}

void DigitalDistortion::CreatePresets()
{
  // order of params is defined by EParams enumeration
  // values are between 0 and 100 because we refer to parameter kThreshold,
  //   NOT our mThreshold member variable
  MakePreset("clean", 0.01);
  MakePreset("slightly distorted", 20.0);
  MakePreset("medium distortion", 40.0);
  MakePreset("high distortion", 80.0);
  MakePreset("MAXIMUM distortion!!!", 100.0);
}
