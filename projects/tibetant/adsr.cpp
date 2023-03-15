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
  _params.push_back(new TxParameterFloat("Attack", 0.01f, 1.f, &_attack, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Decay", 0.01f, 1.f, &_decay, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Sustain", 0.01f, 1.f, &_sustain, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Release", 0.01f, 1.f, &_release, TxParameter::KNOB));
  _params.push_back(new TxParameterBool("Trigger", &_trigger));

  _adsr.setAllTimes(_attack, _decay, _sustain, _release);
}

TxAdsr::~TxAdsr() 
{
}

stk::StkFloat TxAdsr::tick(unsigned int)
{
  _trigger = _params[TxAdsr::TRIGGER]->tick();
  _adsr.setAllTimes(_attack, _decay, _sustain, _release);

  if(_trigger) _adsr.keyOn();
  else _adsr.keyOff();
  
  return _adsr.tick();
}

stk::StkFrames& TxAdsr::tick(stk::StkFrames& frames, unsigned int channel)
{
  _params[TxAdsr::ATTACK]->tick();
  _params[TxAdsr::DECAY]->tick();
  _params[TxAdsr::SUSTAIN]->tick();
  _params[TxAdsr::RELEASE]->tick();

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
  
  TxParameter* attack = _params[TxAdsr::ATTACK];
  attack->draw();
  ImGui::SameLine();
  TxParameter* decay = _params[TxAdsr::DECAY];
  decay->draw();
  ImGui::SameLine();
  TxParameter* sustain = _params[TxAdsr::SUSTAIN];
  sustain->draw();
  ImGui::SameLine();
  TxParameter* release = _params[TxAdsr::RELEASE];
  release->draw();
  
  ImGui::EndGroup();
/*
  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
*/
  ImGui::End();
}