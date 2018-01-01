#include "Synthesis.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <sstream>


const int kNumPrograms = 3;

enum EParams
{
  kFrequency = 0,
  kMode = 3,
  kNumParams
  //Note that kNumParams has no initalizer
  //Why? "If the first enumerator does not have an initializer, the associated value is zero. For any other enumerator whose definition does not have an initializer, the associated value is the value of the previous enumerator plus one." - cppreference
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  // location of Frequency knob on plugin
  kFrequencyX = 200,
  kFrequencyY = 62,
  kKnobFrames = 128
};

Synthesis::Synthesis(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mFrequency(1.), mOscillatorMode(0.)
{
  TRACE;
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kFrequency)->InitDouble("Frequency", 440., 20., 20000.0, 0.01, "Hz");
  // shape determines starting point of knob
  GetParam(kFrequency)->SetShape(2.);
  
  // 4 different oscillator modes
  GetParam(kMode)->InitInt("Mode", 0, 0, 3);
  
  // create the panel
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);
  
  // load the knob sprite and add it to panel
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  
  pGraphics->AttachControl(new IKnobMultiControl(this, kFrequencyX, kFrequencyY, kFrequency, &knob));
  GetParam(kFrequency)->SetDisplayText(440.0, "Frequency");

  
  IText textProps1(18, &COLOR_BLACK, "Helvetica", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
  
  IText textProps2(18, &COLOR_BLACK, "Helvetica", IText::kStyleNormal, IText::kAlignNear, 0, IText::kQualityDefault);
  
  
  // tempRect values (left-edge, top-edge, right-edge, bottom-edge)
  // rectangle goes from 100px to 200px horizontally and from 30px to 50px vertically
  
  IRECT tmpRect1(210, 30, 300, 50);
  showSampleFreq = new ITextControl(this, tmpRect1, &textProps1, "");
  pGraphics->AttachControl(showSampleFreq);
  
  IRECT tmpRect2(210, 200, 300, 220);
  ITextControl* freqTitle = new ITextControl(this, tmpRect2, &textProps1, "Frequency");
  pGraphics->AttachControl(freqTitle);
  
  
  IBitmap bitmap = pGraphics->LoadIBitmap(RADIO_ID, RADIO_FN, 2);// 2 bitmaps: Off and On
  
  const int N_BUTTONS = 4; // Number of radio buttons

  const int X_MIN_RADIO = 25, X_MAX_RADIO = 50;
  const int Y_MIN_RADIO = 40, Y_MAX_RADIO = 180;
  IRECT tmpRect3(X_MIN_RADIO, Y_MIN_RADIO, X_MAX_RADIO, Y_MAX_RADIO);
  
  pGraphics->AttachControl(new IRadioButtonsControl(this, tmpRect3, kMode, N_BUTTONS, &bitmap, kVertical));
  
  std::string options[N_BUTTONS] = {"Sine", "Saw", "Square", "Triangle"};
  
  int v_spacing = (Y_MAX_RADIO - Y_MIN_RADIO)/N_BUTTONS;
  
  for(int i = 0; i < N_BUTTONS; i++){
    int text_line_y = Y_MIN_RADIO+i*(v_spacing+7);
    IRECT tmpRectOption(X_MIN_RADIO+20, text_line_y, X_MAX_RADIO+50, text_line_y+10);
    ITextControl* optionText = new ITextControl(this, tmpRectOption, &textProps2, options[i].c_str());
    pGraphics->AttachControl(optionText);
  }

  
  
  // shows you the borders of your GUI objects
  // pGraphics->ShowControlBounds(true);
  
  UpdateFrequencyText(showSampleFreq);
  
  AttachGraphics(pGraphics);
  
  CreatePresets();
}

Synthesis::~Synthesis() {}

void Synthesis::UpdateFrequencyText(ITextControl* showSampleFreq)
{
  int SampleFreq= GetParam(kFrequency)->Value();
  
  //ITextControl accepts only chars so doubles floats ints etc. need to be converted to char
  std::ostringstream stm;
  stm << SampleFreq;
  stm << "Hz"; // label for frequency
  std::string str1 = stm.str();
  
  char *charSampleFreq = new char[str1.length() + 1];
  strcpy(charSampleFreq, str1.c_str());
  showSampleFreq->SetTextFromPlug(charSampleFreq);
  delete [] charSampleFreq;
}


// process a block of audio

void Synthesis::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  
  double *leftOutput =outputs[0];
  double *rightOutput =outputs[1];
  
  mOscillator.generate(leftOutput, nFrames); // fill left output buffer first
  
  for(int s=0; s<nFrames; s++){
    rightOutput[s] = leftOutput[s]; // fill right output buffer based off left
  }
  
}

void Synthesis::Reset()
{
  TRACE;
  IMutexLock lock(this);
  mOscillator.setSampleRate(GetSampleRate());
}
// handle parameter changes
void Synthesis::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case kFrequency:
      mOscillator.setFrequency(GetParam(kFrequency)->Value());
      // debug message (when we change mFrequency value)
      UpdateFrequencyText(showSampleFreq);
      //DBGMSG( "value of Frequency : %f\n", mFrequency);
      break;
    case kMode:
      mOscillatorMode = GetParam(kMode)->Int();
      mOscillator.setMode(static_cast<OscillatorMode>(mOscillatorMode));
      break;
      
    default:
      break;
  }
}

void Synthesis::CreatePresets()
{
  // order of params is defined by EParams enumeration
  // values are between 0 and 100 because we refer to parameter kFrequency,
  //   NOT our mFrequency member variable

  MakePreset("A4", 440.0, 0);
  MakePreset("A5", 880.0, 0);
  MakePreset("A6", 1760.0, 0);
}
