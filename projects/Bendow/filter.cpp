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
{
  _params.push_back(new TxParameterEnum("Filter", &TxFilter::FilterName[0],
    TxFilter::NumFilter, &_filterIdx));
  _params.push_back(new TxParameterFloat("Gain", 0.f, 1.f, &_gain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float1", -1.f, 1.f, &_float1, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float2", -1.f, 1.f, &_float2, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Float3", -1.f, 1.f, &_float3, TxParameter::KNOB));
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

  switch(filterIdx) {
    case BIQUAD:
      _filter = new stk::BiQuad();
      break;

    case ONEPOLE:
      _filter = new stk::OnePole();
      ((stk::OnePole*)_filter)->setPole(-0.5f);
      break;
  }
  
  _filterIdx = filterIdx;
}

stk::StkFloat TxFilter::tick(unsigned int)
{
  if (!_filter)return 0.f;
  stk::StkFloat sample = _params[TxNode::SAMPLES]->tick();
  stk::StkFloat gain = _params[TxFilter::GAIN]->tick();
  _filter->setGain(gain);
  switch(_filterIdx) {
    case BIQUAD:
      return 0.f;

    case ONEPOLE:
    {
      stk::StkFloat pole = _params[TxFilter::FLOAT1]->tick();
      ((stk::OnePole*)_filter)->setPole(pole);
      return ((stk::OnePole*)_filter)->tick(sample);
    }
    
    default:
      return 0.f;
  }
  
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

  TxParameter* float1 = _params[TxFilter::FLOAT1];
  float1->setLabel("Pole");
  float1->draw();
  ImGui::SameLine();

  switch(_filterIdx) {
    /*
    case BIQUAD:
    {
      
      TxParameter* modFreq = _params[TxFilter::MODFREQUENCY];
      modFreq->draw();
      break;
    }

    case ECHO:
    {
      TxParameter* delay = _params[TxFilter::DELAY];
      delay->draw();
      ImGui::SameLine();
      TxParameter* maxDelay = _params[TxFilter::MAXIMUMDELAY];
      maxDelay->draw();
      break;
    }
    */
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