#include "common.h"
#include "effect.h"

#include <Chorus.h>
#include <Echo.h>


TxEffect::TxEffect(TxGraph* parent, const std::string& name) 
  : TxNode(parent, name)
  , _effect(NULL)
  , _effectIdx(-1)
  , _modDepth(0.5)
  , _modFrequency(6.f)
  , _delay(1)
  , _maximumDelay(500)
{
  _effect = new stk::Echo();
  _effectIdx = ECHO;
  _params.push_back(new TxParameterSamples(this, "Input", false));
  _params.push_back(new TxParameterFloat(this, "ModDepth", 0.f, 1.f, &_modDepth, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "ModFrequency", 1.f, 220.f, &_modFrequency, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "Delay", 0, 1000, &_delay, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "MaximumDelay", 0, 1000, &_maximumDelay, TxParameter::KNOB));

}

TxEffect::~TxEffect() 
{
}

stk::StkFloat TxEffect::tick(unsigned int)
{
  stk::StkFloat sample = _params[TxEffect::INPUT]->tick();
  switch(_effectIdx) {
    case CHORUS:
      ((stk::Chorus*)_effect)->setModDepth(_modDepth);
      ((stk::Chorus*)_effect)->setModFrequency(_modFrequency);
      return ((stk::Chorus*)_effect)->tick(sample);

    case ECHO:
      ((stk::Echo*)_effect)->setDelay(_delay);
      ((stk::Echo*)_effect)->setMaximumDelay(_maximumDelay);
      return ((stk::Echo*)_effect)->tick(sample);
    
    default:
      return 0.f;
  }
  
}

stk::StkFrames& TxEffect::tick(stk::StkFrames& frames, unsigned int channel)
{
  return frames;
}

void TxEffect::_drawImpl(bool* modified)
{

  ImGui::BeginGroup();
  switch(_effectIdx) {
    case CHORUS:
    {
      TxParameter* modDepth = _params[TxEffect::MODDEPTH];
      if (modDepth->draw() && modified)*modified = true;
      ImGui::SameLine();
      TxParameter* modFreq = _params[TxEffect::MODFREQUENCY];
      if (modFreq->draw() && modified)*modified = true;
      break;
    }

    case ECHO:
    {
      TxParameter* delay = _params[TxEffect::DELAY];
      if (delay->draw() && modified)*modified = true;
      ImGui::SameLine();
      TxParameter* maxDelay = _params[TxEffect::MAXIMUMDELAY];
      if (maxDelay->draw() && modified)*modified = true;
      break;
    }

    default:
      break;
  }
  
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  TxNode::_drawCommonControls();

}

void TxEffect::reset()
{

}