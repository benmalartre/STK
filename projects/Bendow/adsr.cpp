#include "common.h"
#include "adsr.h"


TxAdsr::TxAdsr(TxGraph* parent, const std::string& name) 
  : TxNode(parent, name)
  , _attack(0.05f)
  , _decay(0.025f)
  , _sustain(0.9f)
  , _release(0.25f)
  , _trigger(false)
{
  _params.push_back(new TxParameterFloat(this, "Attack", 0.01f, 1.f, &_attack, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Decay", 0.01f, 1.f, &_decay, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Sustain", 0.01f, 1.f, &_sustain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Release", 0.01f, 1.f, &_release, TxParameter::KNOB));
  _params.push_back(new TxParameterBool(this, "Trigger", &_trigger));

  _adsr.setAllTimes(_attack, _decay, _sustain, _release);
  _size = ImVec2(TX_KNOB_SIZE * 5 + TX_PADDING_X * 2, TX_PADDING_Y * 2 + TX_KNOB_SIZE * 2 + TX_SLIDER_SIZE);
}

TxAdsr::~TxAdsr() 
{
}
const ImVec2& TxAdsr::size()
{
  return _size;
}

stk::StkFloat TxAdsr::tick(unsigned int)
{
  _trigger = _params[TxAdsr::TRIGGER]->tick();
  _params[TxAdsr::ATTACK]->tick();
  _params[TxAdsr::DECAY]->tick();
  _params[TxAdsr::SUSTAIN]->tick();
  _params[TxAdsr::RELEASE]->tick();

  _adsr.setAllTimes(_attack, _decay, _sustain, _release);

  if(_trigger) _adsr.keyOn();
  else _adsr.keyOff();
  
  return _adsr.tick();
}

void TxAdsr::setAttack(stk::StkFloat attack)
{
  _attack = attack;
  _adsr.setAttackRate(_attack);
}

void TxAdsr::setDecay(stk::StkFloat decay)
{
  _decay = decay;
  _adsr.setDecayRate(_decay);
}

void TxAdsr::setSustain(stk::StkFloat sustain)
{
  _sustain = sustain;
  _adsr.setSustainLevel(_sustain);
}

void TxAdsr::setRelease(stk::StkFloat release)
{
  _release = release;
  _adsr.setReleaseRate(_release);
}

void TxAdsr::_drawImpl(bool* modified)
{
  ImGui::BeginGroup();
  TxParameter* trigger = _params[TxAdsr::TRIGGER];
  trigger->draw();
  
  TxParameter* attack = _params[TxAdsr::ATTACK];
  if (attack->draw() && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* decay = _params[TxAdsr::DECAY];
  if (decay->draw() && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* sustain = _params[TxAdsr::SUSTAIN];
  if (sustain->draw() && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* release = _params[TxAdsr::RELEASE];
  if (release->draw() && modified)*modified = true;
  
  ImGui::EndGroup();
  TxNode::_drawOutput();
}

void TxAdsr::reset()
{
}