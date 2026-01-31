#ifndef __SLGAIN__
#define __SLGAIN__

#define _USE_MATH_DEFINES
#include <math.h>
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
#include "IPlugMultiTargets_controls.h"


class CParamSmooth {
public:
	CParamSmooth()
	{
		a = 0;
		b = 1. - a;
		z = 0.;
	};

	~CParamSmooth() {};

	inline double Process(double in) {
		z = (in * b) + (z * a);
		return z;
	}

	inline void setFc(double Fc) {
		a = exp(-2.0 * M_PI * Fc);
		b = 1.0 - a;
	}

private:
	double a, b, z;
};

class VolumeKnobControl : public IKnobMultiControlText
{
public:
	VolumeKnobControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap, IText* pText)
		: IKnobMultiControlText(pPlug, pR, paramIdx, pBitmap, pText) {

	}

	void SetNewDefault(double a, double b) {
		{

			a = a * -1;
			float zeroPercentage = (a / (b + a));
			mDefaultValue = zeroPercentage;
		}

	}

	void OnMouseDblClick(int x, int y, IMouseMod* pMod)
	{
		#ifdef RTAS_API
		PromptUserInput(&mTextRECT);
		#else
		if (mDefaultValue >= 0.0)
		{

			VolumeKnobControl::SetValueFromPlug(xYzero); 
			SetDirty();
		}
		#endif
	}

	void setZero(double xy) {
			xYzero = xy;
			return;
	}

	~VolumeKnobControl() {}

private:
	double xYzero = 0.5;

};

class LimitKnobControl : public IKnobMultiControlText
{
public:
	LimitKnobControl(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap, IText* pText)
		: IKnobMultiControlText(pPlug, pR, paramIdx, pBitmap, pText) {}

	void OnMouseDblClick(int x, int y, IMouseMod* pMod)
	{
		#ifdef RTAS_API
		PromptUserInput(&mTextRECT);
		#else
		if (mDefaultValue >= 0.0)
		{

		}
		#endif
	}

	~LimitKnobControl() {}
};


class SLGain : public IPlug
{
public:
  SLGain(IPlugInstanceInfo instanceInfo);
  ~SLGain();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  bool SerializeState(ByteChunk* pChunk);
  int UnserializeState(ByteChunk* pChunk, int startPos);

  float findShape(float dBmin, float dBmax) {

	  double whereIsZero = 0.5;
	  float s;
	  int n = 1;

	  dBmin = dBMin * -1;
	  dBMax = dBmax;

	  float zeroPercentage = (dBmin / (dBmax + dBmin));

	  //if (zeroPercentage <= 0.3)  whereIsZero = 0.2 + zeroPercentage;
	  //if (zeroPercentage >= 0.7)  whereIsZero = 0.8 - (1 - zeroPercentage);
	  zero = whereIsZero;
	  GainControl->setZero(zero);
	 
	  s = n / (log(zero) / log(zeroPercentage));
	  return s;

  }

private:
  CParamSmooth mGainSmoother;
  double mGain;
  VolumeKnobControl * GainControl;
  LimitKnobControl * MinGainControl;
  LimitKnobControl * MaxGainControl;
  double dBMax = 24;
  double dBMin = -96;
  double CurrentGain = 0;
  double zero = 0.5;

};

#endif


