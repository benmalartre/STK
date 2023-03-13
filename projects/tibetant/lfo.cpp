#include "common.h"
#include "lfo.h"


TxLfo::TxLfo(const std::string& name) 
  : TxNode(name)
  , _frequency(0.2f)
  , _amplitude(6.f)
  , _offset(6.f)
{
  _sine.setFrequency(_frequency);
  _params.push_back(new TxParameterFloat("Frequency", 0.01f, 12.f, &_frequency, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Amplitude", 0.01f, 10.f, &_amplitude, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Offset", -100.f, 100.f, &_offset, TxParameter::KNOB));

}

TxLfo::~TxLfo() 
{
}

stk::StkFloat TxLfo::tick(void)
{
  _sine.setFrequency(_frequency);
  return _sine.tick() * _amplitude +_offset;
}

stk::StkFrames& TxLfo::tick(stk::StkFrames& frames, unsigned int channel)
{
  if(_dirty) {
    _sine.tick(frames, 0);
    stk::StkFloat* samples = &frames[0];
    for(size_t f = 0; f < frames.size(); ++f) {
        *samples *= _amplitude;
        *samples++ += _offset;
    }
    //_dirty = false;
  }
  return frames;
}

void TxLfo::setFrequency(stk::StkFloat frequency)
{
  _frequency = frequency;
  _sine.setFrequency(_frequency);
}

void TxLfo::setAmplitude(stk::StkFloat amplitude)
{
  _amplitude = amplitude;
}

void TxLfo::setOffset(stk::StkFloat offset)
{
  _offset = offset;
}

void TxLfo::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128);
  TxParameter* frequency = _params[TxLfo::FREQUENCY];
  frequency->draw();
  ImGui::SameLine();
  TxParameter* amplitude = _params[TxLfo::AMPLITUDE];
  amplitude->draw();
  ImGui::SameLine();
  TxParameter* offset = _params[TxLfo::OFFSET];
  offset->draw();
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
  ImGui::End();
}