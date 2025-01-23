#include "common.h"
#include "random.h"
#include "editor.h"


TxRandom::TxRandom(TxNode* parent, const std::string& name)
  : TxNode(parent, TxNode::RANDOM, name)
  , _minimum(0.f)
  , _maximum(100.f)
  , _seed(0)
  , _cnt(INT_MAX)
  , _frequency(1.f)
{
  _rate = stk::Stk::sampleRate() / _frequency;
  srand(_seed);
  _params.push_back(new TxParameterFloat(this, "Freq", _minimum, _maximum, &_frequency, FREQUENCY, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Min", -1000.f, 1000.f, &_minimum, MINIMUM, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Max", -1000.f, 1000.f, &_maximum, MAXIMUM, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "Seed", 0, 65535, &_seed, SEED, TxParameter::HORIZONTAL));
  _size = ImVec2(TX_KNOB_MIDDLE_SIZE * 4 + TX_PADDING_X * 5, TX_PADDING_Y * 2 + TX_KNOB_MIDDLE_SIZE * 2 + TX_SLIDER_SIZE);

}

TxRandom::~TxRandom()
{

}

const ImVec2& TxRandom::size()
{
  return _size;
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

stk::StkFloat TxRandom::tick(unsigned int channel)
{
  if(!_dirty)return _frames[channel];

  if(_lastFrequency != _frequency)setFrequency(_frequency);
  if(_cnt++ > _rate) {
    _cnt = 0;
    _frames[channel] = RANDOM_LO_HI(_minimum, _maximum);
  }
  _dirty = false;
  return _frames[channel];
}

void TxRandom::_drawImpl(TxEditor* editor, bool* modified)
{
  ImGui::BeginGroup();

  TxNode::_drawAlignLeft(editor);
  TxParameter* frequency = _params[TxRandom::FREQUENCY];
  if(frequency->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* minimum = _params[TxRandom::MINIMUM];
  if(minimum->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* maximum = _params[TxRandom::MAXIMUM];
  if(maximum->draw(editor) && modified)*modified = true;

  ImGui::SetNextItemWidth(128 * editor->scale());
  TxParameter* seed = _params[TxRandom::SEED];
  if(seed->draw(editor) && modified)*modified = true;

  ImGui::EndGroup();

  TxNode::_drawInput(editor, _params[TxRandom::FREQUENCY], 0);
  TxNode::_drawInput(editor, _params[TxRandom::MINIMUM], 1);
  TxNode::_drawInput(editor, _params[TxRandom::MAXIMUM], 2);
  TxNode::_drawInput(editor, _params[TxRandom::SEED], 3);

  TxNode::_drawOutput(editor);
}

void TxRandom::reset()
{
  srand(_seed);
}