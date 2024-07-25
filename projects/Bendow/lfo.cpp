#include "common.h"
#include "lfo.h"
#include "graph.h"


TxLfo::TxLfo(TxGraph* parent, const std::string& name) 
  : TxNode(parent, name)
  , _frequency(0.2f)
  , _amplitude(6.f)
  , _offset(6.f)
{
  _sine.setFrequency(_frequency);
  _params.push_back(new TxParameterFloat(this, "Frequency", 0.01f, 12.f, &_frequency, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Amplitude", 0.01f, 10.f, &_amplitude, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Offset", -100.f, 100.f, &_offset, TxParameter::KNOB));
  //_buffer.scale(-10, 10);
  _size = ImVec2(TX_KNOB_SIZE * 4 + TX_PADDING_X * 2, TX_PADDING_Y * 2 + TX_KNOB_SIZE * 2);
}

TxLfo::~TxLfo() 
{
}

const ImVec2& TxLfo::size()
{
  return _size;
}

stk::StkFloat TxLfo::tick(unsigned int)
{
  _sine.setFrequency(_frequency);
  const stk::StkFloat sample = _sine.tick() * _amplitude +_offset;
  //_buffer.write(sample);
  return sample;
}

void TxLfo::_drawImpl(bool* modified)
{
  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128 * _parent->scale());
  TxParameter* frequency = _params[TxLfo::FREQUENCY];
  if(frequency->draw() && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* amplitude = _params[TxLfo::AMPLITUDE];
  if(amplitude->draw() && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* offset = _params[TxLfo::OFFSET];
  if(offset->draw() && modified)*modified = true;
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  TxNode::_drawOutput();
}

void TxLfo::reset()
{
  _sine.reset();
}