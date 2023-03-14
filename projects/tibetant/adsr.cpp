#include "common.h"
#include "adsr.h"

TxAdsr::TxAdsr(const std::string& name) 
  : TxNode(name)
  , _attack(0.05f)
  , _decay(0.025f)
  , _sustain(0.9f)
  , _release(0.25f)
  , _trigger(false)
{
  _params.push_back(new TxParameterFloat("Attack", 0.f, 1.f, &_attack, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Decay", 0.f, 1.f, &_decay, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Sustain", 0.f, 1.f, &_sustain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Release", 0.f, 1.f, &_release, TxParameter::KNOB));
  _params.push_back(new TxParameterBool("Trigger", &_trigger));

  _adsr.setAllTimes(_attack, _decay, _sustain, _release);
}

TxAdsr::~TxAdsr() 
{
}

stk::StkFloat TxAdsr::tick(unsigned int)
{
  _trigger = _params[TxAdsr::TRIGGER]->tick();
  if(_trigger) _adsr.keyOn();
  else _adsr.keyOff();
  
  return _adsr.tick();
}

stk::StkFrames& TxAdsr::tick(stk::StkFrames& frames, unsigned int channel)
{
  _adsr.setAllTimes(_attack, _decay, _sustain, _release);

  stk::StkFloat* samples = &frames[0];
  for(size_t f = 0; f < frames.size(); ++f) {
    _trigger = _params[TxAdsr::TRIGGER]->tick();
    if(_trigger) _adsr.keyOn();
    else _adsr.keyOff();

    *samples++ *= _adsr.tick();
  }
  return frames;
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

void TxAdsr::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();
  ImGui::Checkbox("Trigger", &_trigger);
  /*
  ImGui::SetNextItemWidth(128);
  TxParameter* frequency = _params[TxAdsr::FREQUENCY];
  frequency->draw();
  ImGui::SameLine();
  TxParameter* amplitude = _params[TxAdsr::AMPLITUDE];
  amplitude->draw();
  ImGui::SameLine();
  TxParameter* offset = _params[TxAdsr::OFFSET];
  offset->draw();
  */
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
  ImGui::End();
}