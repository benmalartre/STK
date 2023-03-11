#include "common.h"
#include "lfo.h"


TxLfo::TxLfo(const std::string& name) 
  : TxNode(name)
  , _frequency(2.f)
  , _amplitude(2.f)
  , _offset(60.f)
{
  _sine.setFrequency(_frequency);
}

TxLfo::~TxLfo() 
{
}

stk::StkFrames& TxLfo::tick()
{
  if(_dirty) {
    _sine.tick(_frames, 0);
    stk::StkFloat* samples = &_frames[0];
    for(size_t f = 0; f < _frames.size(); ++f) {
        *samples *= _amplitude;
        *samples++ += _offset;
    }
    //_dirty = false;
  }
  return _frames;
}

void TxLfo::setFrequency(const stk::StkFloat& frequency)
{
  _frequency = frequency;
  _sine.setFrequency(_frequency);
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