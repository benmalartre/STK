#include "common.h"
#include "filter.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <BiQuad.h>
#include <Delay.h>
#include <DelayA.h>
#include <DelayL.h>
#include <Fir.h>
#include <FormSwep.h>
#include <Iir.h>
#include <OnePole.h>
#include <OneZero.h>
#include <PoleZero.h>
#include <TwoPole.h>
#include <TwoZero.h>
#include <TapDelay.h>

ImVec2 TxFilter::Size(512, 256);

const char* TxFilter::FilterName[TxFilter::NumFilter] = {
  "Biquad",
  "Delay",
  "DelayA",
  "DelayL",
  "Fir",
  "Iir",
  "FormSwep",
  "OnePole",
  "OneZero",
  "PoleZero",
  "TwoPole",
  "TwoZero",
  "TapDelay"
};

  enum Type {
    BIQUAD,
    DELAY,
    DELAYA,
    DELAYL,
    FIR,
    IIR,
    FORMSWEP,
    ONEPOLE,
    ONEZERO,
    POLEZERO,
    TWOPOLE,
    TWOZERO,
    TAPDELAY
  };


TxFilter::TxFilter(const std::string& name) 
  : TxNode(name)
  , _filter(NULL)
  , _filterIdx(-1)
  , _gain(0.9f)
  , _float1(1.f)
  , _float2(0.6f)
  , _float3(0.3f)
  , _int1(0)
  , _int2(0)
  , _int3(0)
{
  _params.push_back(new TxParameterEnum("Filter", &TxFilter::FilterName[0],
    TxFilter::NumFilter, &_filterIdx));
  _params.push_back(new TxParameterFloat("Gain", 0.f, 1.f, &_gain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float1", -1.f, 1.f, &_float1, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float2", -1.f, 1.f, &_float2, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float3", -1.f, 1.f, &_float3, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("Int1", 0, 12, &_int1, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("Int2", 0, 12, &_int2, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("Int3", 0, 12, &_int3, TxParameter::KNOB));
  _params.push_back(new TxParameterBool("Bool1", &_bool1));
  setFilter(ONEPOLE);

}

TxFilter::~TxFilter() 
{
}

const ImVec2& TxFilter::size()
{
  return TxFilter::Size;
}
/*
* This function calculates the zeroth order Bessel function
*/
static double Ino(double x)
{
  double d = 0, ds = 1, s = 1;
  do
  {
    d += 2;
    ds *= x*x/(d*d);
    s += ds;
  }
  while (ds > s*1e-6);
  return s;
}

std::vector<float> computeFirCoefficients(int Fs, float Fa, float Fb, int Att, int M)
{
  /*
    * This function calculates Kaiser windowed
    * FIR filter coefficients for a single passband
    * based on
    * "DIGITAL SIGNAL PROCESSING, II" IEEE Press pp 123-126.
    *
    * Fs=Sampling frequency
    * Fa=Low freq ideal cut off (0=low pass)
    * Fb=High freq ideal cut off (Fs/2=high pass)
    * Att=Minimum stop band attenuation (>21dB)
    * M=Number of points in filter (ODD number)
    * return the output coefficients (they are symetric only half generated)
    */
  int j,  Np = (M-1)/2;

  float alpha, inoAlpha;
  std::vector<float> A(Np+1);
  std::vector<float> coeffs(Np*2+1);

  // Calculate the impulse response of the ideal filter
  A[0] = 2*(Fb-Fa)/Fs;
  for (j=1; j<=Np; j++)
  {
    A[j] = (std::sinf(2*j*M_PI*Fb/Fs)-std::sin(2*j*M_PI*Fa/Fs))/(j*M_PI);
  }
  // Calculate the desired shape factor for the Kaiser-Bessel window
  if (Att<21)
  {
    alpha = 0;
  }
  else if (Att>50)
  {
    alpha = 0.1102*(Att-8.7);
  }
  else
  {
    alpha = 0.5842*std::pow((Att-21), 0.4)+0.07886*(Att-21);
  }
  // Window the ideal response with the Kaiser-Bessel window
  inoAlpha = Ino(alpha);
  for (j=0; j<=Np; j++)
  {
    coeffs[Np+j] = A[j]*Ino(alpha*std::sqrt(1-(j*j/(Np*Np))))/inoAlpha;
  }
  for (j=0; j<Np; j++)
  {
    coeffs[j] = coeffs[M-1-j];
  }

  return coeffs;
};

void TxFilter::setFilter(int filterIdx)
{
  if(_filter)delete _filter;
  _filter = NULL;
  _filterIdx = filterIdx;

  TxParameterFloat* float1 = (TxParameterFloat*)_params[FLOAT1];
  TxParameterFloat* float2 = (TxParameterFloat*)_params[FLOAT2];
  TxParameterFloat* float3 = (TxParameterFloat*)_params[FLOAT3];
  TxParameterInt* int1 = (TxParameterInt*)_params[INT1];
  TxParameterInt* int2 = (TxParameterInt*)_params[INT2];
  TxParameterInt* int3 = (TxParameterInt*)_params[INT3];
  TxParameterBool* bool1 = (TxParameterBool*)_params[BOOL1];

  switch(filterIdx) {
    case BIQUAD:
      _filter = new stk::BiQuad();
      float1->setMinimum(20.f);
      float1->setMaximum(30000.f);
      float1->set(220.f);
      float1->setLabel("Resonance");
      float2->setMinimum(0.f);
      float2->setMaximum(1.f);
      float2->set(0.5f);
      float2->setLabel("Radius");
      bool1->set(true);
      bool1->setLabel("Normalize");
      ((stk::BiQuad*)_filter)->setResonance(float1->tick(), float2->tick(), bool1->tick());
      //((stk::BiQuad*)_filter)->setNotch(0.2);
      break;

    case ONEPOLE:
      _filter = new stk::OnePole();
      float1->setMinimum(-0.999f);
      float1->setMaximum(0.999);
      float1->set(0.5);
      float1->setLabel("Pole");
      ((stk::OnePole*)_filter)->setPole(-0.5f);
      break;

    case DELAY:_filter = new stk::Delay();
      int1->setMinimum(0);
      int1->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int1->set(4096.f);
      int1->setLabel("Maximum");
      int2->setMinimum(0);
      int2->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int2->set(0.f);
      int2->setLabel("Delay");
      ((stk::Delay*)_filter)->setMaximumDelay(int1->tick());
      ((stk::Delay*)_filter)->setDelay(int2->tick());
      break;

    case DELAYA:
      _filter = new stk::DelayA();
      int1->setMinimum(0);
      int1->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int1->set(4096.f);
      int1->setLabel("Maximum");
      int2->setMinimum(0);
      int2->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int2->set(0.f);
      int2->setLabel("Delay");
      ((stk::DelayA*)_filter)->setMaximumDelay(int1->tick());
      ((stk::DelayA*)_filter)->setDelay(int2->tick());
      break;

    case DELAYL:
      _filter = new stk::DelayL();
      int1->setMinimum(0);
      int1->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int1->set(4096.f);
      int1->setLabel("Maximum");
      int2->setMinimum(0);
      int2->setMaximum((int)stk::Stk::sampleRate() * 0.5);
      int2->set(0.f);
      int2->setLabel("Delay");
      ((stk::DelayL*)_filter)->setMaximumDelay(int1->tick());
      ((stk::DelayL*)_filter)->setDelay(int2->tick());
      break;

    case FIR:
      _filter = new stk::Fir();
      int1->setMinimum(4);
      int1->setMaximum(1024);
      int1->set(64);
      int1->setLabel("N");
      int2->setMinimum(21);
      int2->setMaximum(1000);
      int2->set(64);
      int2->setLabel("Db");
      float1->setMinimum(0.f);
      float1->setMaximum(20000.f);
      float1->set(220.f);
      float1->setLabel("Low");
      float2->setMinimum(0.f);
      float2->setMaximum(60000.f);
      float2->set(4400.f);
      float2->setLabel("High");
      std::vector<float> coeffs = computeFirCoefficients(stk::Stk::sampleRate(), 
        float1->tick(), float2->tick(), int2->tick(), int1->tick());

      ((stk::Fir*)_filter)->setCoefficients(coeffs, true);
      break;
  }
  
  _lastFilterIdx = _filterIdx;
}

stk::StkFloat TxFilter::tick(unsigned int)
{
  if (!_filter)return 0.f;
  if (_params[FILTER]->tick() != _lastFilterIdx) {
    setFilter(_filterIdx);
  }
  stk::StkFloat input = _params[TxNode::SAMPLES]->tick();
  stk::StkFloat gain = _params[TxFilter::GAIN]->tick();
  stk::StkFloat sample = 0.f;
  _filter->setGain(gain);
  switch(_filterIdx) {
    case BIQUAD:
    {
      stk::StkFloat resonance = _params[TxFilter::FLOAT1]->tick();
      stk::StkFloat radius = _params[TxFilter::FLOAT2]->tick();
      bool normalize = _params[TxFilter::BOOL1]->tick();
      ((stk::BiQuad*)_filter)->setResonance(resonance, radius, normalize);
      sample = ((stk::BiQuad*)_filter)->tick(input);
      break;
    }

    case ONEPOLE:
    {
      stk::StkFloat pole = _params[TxFilter::FLOAT1]->tick();
      ((stk::OnePole*)_filter)->setPole(pole);
      sample = ((stk::OnePole*)_filter)->tick(input);
      break;
    }

    case DELAY:
    {
      int maxDelay = _params[TxFilter::INT1]->tick();
      int delay = _params[TxFilter::INT2]->tick();
      ((stk::Delay*)_filter)->setMaximumDelay(maxDelay);
      ((stk::Delay*)_filter)->setDelay(delay);
      sample = ((stk::Delay*)_filter)->tick(input);
      break;
    }

    case DELAYA:
    {
      int maxDelay = _params[TxFilter::INT1]->tick();
      int delay = _params[TxFilter::INT2]->tick();
      ((stk::DelayA*)_filter)->setMaximumDelay(maxDelay);
      ((stk::DelayA*)_filter)->setDelay(delay);
      sample = ((stk::DelayA*)_filter)->tick(input);
      break;
    }

    case DELAYL:
    {
      int maxDelay = _params[TxFilter::INT1]->tick();
      int delay = _params[TxFilter::INT2]->tick();
      ((stk::DelayL*)_filter)->setMaximumDelay(maxDelay);
      ((stk::DelayL*)_filter)->setDelay(delay);
      sample = ((stk::DelayL*)_filter)->tick(input);
      break;
    }

    case FIR:
    {
      
      ((stk::Fir*)_filter)->setCoefficients(
        computeFirCoefficients(stk::Stk::sampleRate(),
          _params[FLOAT1]->tick(), _params[FLOAT2]->tick(), 
          _params[INT2]->tick(), _params[INT1]->tick()), false);
      
      sample = ((stk::Fir*)_filter)->tick(input);
      break;
    }
  }

  _buffer.write(sample);
  return sample;
}

stk::StkFrames& TxFilter::tick(stk::StkFrames& frames, unsigned int channel)
{
  return frames;
}

void TxFilter::_drawImpl(bool* modified)
{
  TxNode::drawInput();
  ImGui::BeginGroup();

  ImGui::SetNextItemWidth(128);
  TxParameter* filter = _params[TxFilter::FILTER];
  if(filter->draw()&&modified)*modified=true;

  TxParameter* gain = _params[TxFilter::GAIN];
  if (gain->draw() && modified)*modified = true;
  ImGui::SameLine();

  switch(_filterIdx) {
    case BIQUAD:
      if(_params[TxFilter::FLOAT1]->draw() && modified)*modified=true;
      ImGui::SameLine();
      if(_params[TxFilter::FLOAT2]->draw() && modified)* modified = true;
      if(_params[TxFilter::BOOL1]->draw() && modified)* modified = true;
      break;

    case ONEPOLE:
      if(_params[TxFilter::FLOAT1]->draw() && modified)*modified = true;
      break;
  
    case DELAY:
    case DELAYA:
    case DELAYL:
      if(_params[TxFilter::INT1]->draw() && modified)*modified = true;
      ImGui::SameLine();
      if(_params[TxFilter::INT2]->draw() && modified)*modified = true;
      break;

    case FIR:
      if(_params[TxFilter::INT1]->draw() && modified)*modified = true;
      ImGui::SameLine();
      if(_params[TxFilter::FLOAT1]->draw() && modified)*modified = true;
      ImGui::SameLine();
      if(_params[TxFilter::FLOAT2]->draw() && modified)*modified = true;
      ImGui::SameLine();
      if(_params[TxFilter::INT2]->draw() && modified)*modified = true;
      break;
      
    default:
      break;
  }
  
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
}

void TxFilter::reset()
{

}