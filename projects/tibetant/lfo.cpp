#include "common.h"
#include "lfo.h"


TxLfo::TxLfo(const std::string& name) 
  : TxNode(name)
  , _frequency(0.2f)
  , _amplitude(6.f)
  , _offset(6.f)
{
  _sine.setFrequency(_frequency);
}

TxLfo::~TxLfo() 
{
}

stk::StkFloat TxLfo::tick(void)
{
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
  
  if (ImGuiKnobs::Knob("Frequency", &_frequency, 20.f, 2000.f, 1.f, "%.1fHz", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    setFrequency(_frequency);
  }
  ImGui::SameLine();
  
  ImGui::BeginGroup();
  
  if (ImGui::SliderFloat("Stereo", &_stereo, -1.f, 1.f)) {
  }


  ImGui::EndGroup();
  ImGui::End();
}