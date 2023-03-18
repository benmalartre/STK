#include "common.h"
#include "filter.h"

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
  , _float1(0.5f)
  , _float2(1.f)
  , _float3(0.f)
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
  /*
  _params.push_back(new TxParameterFloat("ModDepth", 0.f, 1.f, &_modDepth, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("ModFrequency", 1.f, 220.f, &_modFrequency, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("Delay", 0, 1000, &_delay, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("MaximumDelay", 0, 1000, &_maximumDelay, TxParameter::KNOB));
  */
}

TxFilter::~TxFilter() 
{
}

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

    case DELAY:
      _filter = new stk::Delay();
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
  }

  _buffer.write(sample);
  return sample;
}

stk::StkFrames& TxFilter::tick(stk::StkFrames& frames, unsigned int channel)
{
  return frames;
}

void TxFilter::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();

  ImGui::SetNextItemWidth(128);
  TxParameter* filter = _params[TxFilter::FILTER];
  filter->draw();

  TxParameter* gain = _params[TxFilter::GAIN];
  gain->draw();
  ImGui::SameLine();

  switch(_filterIdx) {
    case BIQUAD:
      _params[TxFilter::FLOAT1]->draw();
      ImGui::SameLine();
      _params[TxFilter::FLOAT2]->draw();
      _params[TxFilter::BOOL1]->draw();
      break;

    case ONEPOLE:
      _params[TxFilter::FLOAT1]->draw();
      break;
  
    case DELAY:
      _params[TxFilter::INT1]->draw();
      ImGui::SameLine();
      _params[TxFilter::INT2]->draw();
      break;

    default:
      break;
  }
  
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();

  ImGui::End();
}

void TxFilter::reset()
{

}