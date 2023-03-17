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
  , _gain(0.9f)
{
  _filter = new stk::OnePole();
  ((stk::OnePole*)_filter)->setPole(-0.5f);
  _filterIdx = ONEPOLE;

  _params.push_back(new TxParameterFloat("Gain", 0.f, 1.f, &_gain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Pole", -1.f, 1.f, &_pole, TxParameter::KNOB));
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

stk::StkFloat TxFilter::tick(unsigned int)
{
  stk::StkFloat sample = _params[TxNode::SAMPLES]->tick();
  stk::StkFloat gain = _params[TxFilter::GAIN]->tick();
  _filter->setGain(gain);
  switch(_filterIdx) {
    case BIQUAD:
      return 0;

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