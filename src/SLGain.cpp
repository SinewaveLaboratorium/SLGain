#include "..\..\WDL\IPlug\IControl.h"
#include "SLGain.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"


const int kNumPrograms = 1;

float AmplitudeTodB(float amplitude)
{
	return 20.0f * log10(amplitude);
}
float dBToAmplitude(float dB)
{
	return pow(10.0f, dB / 20.0f);
}

enum EParams
{
  kminGain = 0,
  kmaxGain = 1,
  kGain = 2,
  kNumParams = 3
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kGainX = 35,
	kGainY = 45,
	kKnobFrames = 60,
	kMinGainX = 10,
	kMinGainY = 95,
	kKnobFrames2 = 1,
	kMaxGainX = 98,
	kMaxGainY = 95,

};

SLGain::SLGain(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), 
	mGain(1.)
{
  TRACE;
 
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kminGain)->InitDouble("Min Gain", dBMin, -96., -6.0, 0.1, "dB");
  GetParam(kmaxGain)->InitDouble("Max Gain", dBMax, 6.0, 48., 0.1, "dB");
  GetParam(kGain)->InitDouble("Gain", CurrentGain, dBMin, dBMax, 0.1, "dB");

  
 

  IGraphics* pGraphics;
  IBitmap knob, minknob, maxknob;
  IText Text1(12, &COLOR_BLACK, "Arial", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);

  pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  minknob = pGraphics->LoadIBitmap(KNOB1_ID, TEXTINPUT_FN, kKnobFrames2);
  maxknob = pGraphics->LoadIBitmap(KNOB2_ID, TEXTINPUT_FN, kKnobFrames2);

  IRECT knobGaindB_RECT(kGainX, kGainY, kGainX + 60, kGainY + 65 + 20);
  GainControl = new VolumeKnobControl(this, knobGaindB_RECT, kGain, &knob, &Text1);
  double shape = findShape(dBMin, dBMax);
  GetParam(kGain)->SetShape(shape);
  pGraphics->AttachControl(GainControl); // Realtime value display under knob

  IRECT MinRect(kMinGainX, kMinGainY, kMinGainX + 25, kMinGainY + 35);
  MinGainControl = new LimitKnobControl(this, MinRect, kminGain, &minknob, &Text1);
  pGraphics->AttachControl(MinGainControl); // Realtime value display of min value

  IRECT MaxRect(kMaxGainX, kMaxGainY, kMaxGainX + 25, kMaxGainY + 35);
  MaxGainControl = new LimitKnobControl(this, MaxRect, kmaxGain, &maxknob, &Text1);
  pGraphics->AttachControl(MaxGainControl); // Realtime value display of max value

  GainControl->SetNewDefault(dBMin, dBMax);

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  }

SLGain::~SLGain() {}

void SLGain::Reset()
{
	TRACE;
	IMutexLock lock(this);

	mGainSmoother.setFc(5 / GetSampleRate());
}



void SLGain::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
	  *out1 = *in1 * mGainSmoother.Process(mGain);
	  *out2 = *in2 * mGainSmoother.Process(mGain);
  }
}

void SLGain::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
  case kGain: {
		mGain = dBToAmplitude(GetParam(kGain)->Value()); 
		CurrentGain = GetParam(kGain)->Value();
		GetParam(kGain)->InitDouble("Gain", CurrentGain, dBMin, dBMax, 0.1, "dB");
		GainControl->SetNewDefault(dBMin, dBMax);
		double shape = findShape(dBMin, dBMax);
		GetParam(kGain)->SetShape(shape);
		
		break; 
		}

  case kminGain: {
	  dBMin = GetParam(kminGain)->Value();
	  if (CurrentGain <= dBMin) { CurrentGain = dBMin; };
	  GetParam(kGain)->InitDouble("Gain", CurrentGain, dBMin, dBMax, 0.1, "dB");
	  mGain = dBToAmplitude(GetParam(kGain)->Value());
	  double shape = findShape(dBMin, dBMax);
	  GetParam(kGain)->SetShape(shape);
	  GainControl->SetNewDefault(dBMin, dBMax);
	  
	  RedrawParamControls();
	  break;
		}

  case kmaxGain: {
	  dBMax = GetParam(kmaxGain)->Value();
	  if (CurrentGain >= dBMax) { CurrentGain = dBMax; };
	  GetParam(kGain)->InitDouble("Gain", CurrentGain, dBMin, dBMax, 0.1, "dB");
	  mGain = dBToAmplitude(GetParam(kGain)->Value());
	  double shape = findShape(dBMin, dBMax);
	  GetParam(kGain)->SetShape(shape);
	  GainControl->SetNewDefault(dBMin, dBMax);

	  RedrawParamControls();
	  break;
		}

  default: break; 
  }
}

bool SLGain::SerializeState(ByteChunk * pChunk)
{
	TRACE;
	IMutexLock lock(this);

	double v0 = GetParam(kminGain)->Value();
	double v1 = GetParam(kmaxGain)->Value();
	//double v2 = GetParam(kGain)->Value();
	// double v2 = CurrentGain;
	double v2 = CurrentGain;

	pChunk->Put(&v0);
	pChunk->Put(&v1);
	pChunk->Put(&v2);

	return IPlugBase::SerializeParams(pChunk);
}

int SLGain::UnserializeState(ByteChunk * pChunk, int startPos)
{
	TRACE;
	IMutexLock lock(this);

	double v = 0.0;
	
	startPos = pChunk->Get(&v, startPos);
	dBMin = startPos;
	startPos = pChunk->Get(&v, startPos);
	dBMax = startPos;
	startPos = pChunk->Get(&v, startPos);
	CurrentGain = v;

	return IPlugBase::UnserializeParams(pChunk, startPos); // must remember to call UnserializeParams at the end
}


