#include "common.h"
#include "random.h"

TxRandom::TxRandom(const std::string& name)
  : TxNode(name)
  , _minimum(-1.f)
  , _maximum(1.f)
  , _seed(0)
  , _cnt(INT_MAX)
  , _frequency(1.f)
{
  _rate = stk::Stk::sampleRate() / _frequency;
  srand(_seed);
  _params.push_back(new TxParameterFloat("Frequency", _minimum, _maximum, &_frequency, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Minimum", -1000.f, 1000.f, &_minimum, TxParameter::KNOB));
  _params.back()->setCallback(new TxCallback((CALLBACK_FN)&updateRandomBounds, this));
  _params.push_back(new TxParameterFloat("Maximum", -1000.f, 1000.f, &_maximum, TxParameter::KNOB));
  _params.back()->setCallback(new TxCallback((CALLBACK_FN)&updateRandomBounds, this));
  _params.push_back(new TxParameterInt("Seed", 0, 65535, &_seed, TxParameter::HORIZONTAL));
}

TxRandom::~TxRandom()
{

}

void TxRandom::updateBounds()
{
  TxParameterFloat* frequency = (TxParameterFloat*)_params[TxRandom::FREQUENCY];
  frequency->setMinimum(_minimum);
  frequency->setMaximum(_maximum);
}

void TxRandom::setMinimum(stk::StkFloat value)
{
  _minimum = value;
}

void TxRandom::setMaximum(stk::StkFloat value)
{
  _maximum = value;
}

void TxRandom::setSeed(int seed)
{
  _seed = seed;
  srand(_seed);
}

void TxRandom::setFrequency(stk::StkFloat frequency)
{
  _frequency = frequency;
  _lastFrequency = _frequency;
  _rate = stk::Stk::sampleRate()  / _frequency;
}

stk::StkFloat TxRandom::tick(void)
{
  if(_lastFrequency != _frequency)setFrequency(_frequency);
  if(_cnt++ > _rate) {
    _cnt = 0;
    _frames[0] = RANDOM_LO_HI(_minimum, _maximum);;
  }
  return _frames[0];
}

stk::StkFrames& TxRandom::tick(stk::StkFrames& frames, unsigned int channel)
{
  if(_active) {
    for(size_t i = 0; i < frames.size(); ++i) {
      if(_cnt++ > _rate) {
        _cnt = 0;
        _frames[0] = RANDOM_LO_HI(_minimum, _maximum);;
      }
      frames[i] = _frames[0];
    }
  } else {
    for(size_t i = 0; i < frames.size(); ++i) {
      frames[i] = _frames[0];
    }
  }
  return frames;
}

void TxRandom::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128);
  TxParameter* seed = _params[TxRandom::SEED];
  seed->draw();
  TxParameter* frequency = _params[TxRandom::FREQUENCY];
  frequency->draw();
  ImGui::SameLine();
  TxParameter* minimum = _params[TxRandom::MINIMUM];
  minimum->draw();
  ImGui::SameLine();
  TxParameter* maximum = _params[TxRandom::MAXIMUM];
  maximum->draw();
  
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
  ImGui::End();
}