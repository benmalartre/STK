#include "common.h"
#include "lfo.h"
#include "graph.h"
#include "editor.h"


TxLfo::TxLfo(TxNode* parent, const std::string& name) 
  : TxNode(parent, TxNode::LFO, name)
  , _frequency(0.2f)
  , _amplitude(6.f)
  , _offset(6.f)
{
  _sine.setFrequency(_frequency);
  _params.push_back(new TxParameterFloat(this, "Freq", 0.01f, 1000.f, &_frequency, FREQUENCY, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Ampl", 0.01f, 100.f, &_amplitude, AMPLITUDE, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Zero", -1000.f, 1000.f, &_offset, OFFSET, TxParameter::KNOB));
  _size = ImVec2(TX_KNOB_MIDDLE_SIZE * 3 + TX_PADDING_X * 4, TX_PADDING_Y * 2 + TX_KNOB_MIDDLE_SIZE * 2);
}

TxLfo::~TxLfo() 
{
}

const ImVec2& TxLfo::size()
{
  return _size;
}

stk::StkFloat TxLfo::tick(unsigned int channel)
{
  if(!_dirty) return _frames[channel];

  _frequency = _params[FREQUENCY]->tick();
  _amplitude = _params[AMPLITUDE]->tick();
  _offset = _params[OFFSET]->tick();

  _sine.setFrequency(_frequency);
  _frames[channel] = _sine.tick() * _amplitude +_offset;
  _buffer.write(_frames[channel]);
  _dirty = false;
  return _frames[channel];
}

void TxLfo::_drawImpl(TxEditor* editor, bool* modified)
{
  ImGui::BeginGroup();

  TxNode::_drawAlignLeft(editor);
  TxParameter* frequency = _params[TxLfo::FREQUENCY];
  if(frequency->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* amplitude = _params[TxLfo::AMPLITUDE];
  if(amplitude->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* offset = _params[TxLfo::OFFSET];
  if(offset->draw(editor) && modified)*modified = true;
  ImGui::EndGroup();

  TxNode::_drawInput(editor, _params[TxLfo::FREQUENCY], 0);
  TxNode::_drawInput(editor, _params[TxLfo::AMPLITUDE], 1);
  TxNode::_drawInput(editor, _params[TxLfo::OFFSET], 2);

  TxNode::_drawOutput(editor);
}

void TxLfo::reset()
{
  _sine.reset();
}