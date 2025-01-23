#include "common.h"
#include "effect.h"

#include <Chorus.h>
#include <Echo.h>

const ImVec2 TxEffect::Size(300, 250);

TxEffect::TxEffect(TxNode* parent, const std::string& name) 
  : TxNode(parent, TxNode::EFFECT, name)
  , _effect(NULL)
  , _effectIdx(-1)
  , _modDepth(0.5)
  , _modFrequency(6.f)
  , _delay(1)
  , _maximumDelay(500)
{
  _effect = new stk::Echo();
  _effectIdx = ECHO;
  _params.push_back(new TxParameterSamples(this, "Input", INPUT, false));
  _params.push_back(new TxParameterFloat(this, "ModDpth", 0.f, 1.f, &_modDepth, MODDEPTH, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "ModFreq", 1.f, 220.f, &_modFrequency, MODFREQUENCY, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "Delay", 0, 1000, &_delay, DELAY, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "MaxDelay", 0, 1000, &_maximumDelay, MAXIMUMDELAY, TxParameter::KNOB));
}

TxEffect::~TxEffect() 
{
}

const ImVec2& TxEffect::size()
{
  return TxEffect::Size;
}

stk::StkFloat TxEffect::tick(unsigned int channel)
{
  if(!_dirty) return _frames[channel];

  stk::StkFloat sample = _params[TxEffect::INPUT]->tick();
  switch(_effectIdx) {
    case CHORUS:
      ((stk::Chorus*)_effect)->setModDepth(_modDepth);
      ((stk::Chorus*)_effect)->setModFrequency(_modFrequency);
      _frames[channel] = ((stk::Chorus*)_effect)->tick(sample);

    case ECHO:
      ((stk::Echo*)_effect)->setDelay(_delay);
      ((stk::Echo*)_effect)->setMaximumDelay(_maximumDelay);
      _frames[channel] = ((stk::Echo*)_effect)->tick(sample);
    
    default:
      _frames[channel] = 0.f;
  }

  _dirty = false;
  return _frames[channel];
}

void TxEffect::_drawImpl(TxEditor* editor, bool* modified)
{

  ImGui::BeginGroup();
  switch(_effectIdx) {
    case CHORUS:
    {
      TxParameter* modDepth = _params[TxEffect::MODDEPTH];
      if (modDepth->draw(editor) && modified)*modified = true;
      ImGui::SameLine();
      TxParameter* modFreq = _params[TxEffect::MODFREQUENCY];
      if (modFreq->draw(editor) && modified)*modified = true;
      break;
    }

    case ECHO:
    {
      TxParameter* delay = _params[TxEffect::DELAY];
      if (delay->draw(editor) && modified)*modified = true;
      ImGui::SameLine();
      TxParameter* maxDelay = _params[TxEffect::MAXIMUMDELAY];
      if (maxDelay->draw(editor) && modified)*modified = true;
      break;
    }

    default:
      break;
  }
  
 TxNode::_drawInput(editor, _params[TxEffect::INPUT], 0);
  
  ImGui::EndGroup();


  TxNode::_drawOutput(editor);

}

void TxEffect::reset()
{

}