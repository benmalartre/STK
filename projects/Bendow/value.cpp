#include "value.h"

TxValue::TxValue(TxGraph* parent, const std::string& name)
  : TxNode(parent, name)
{
  
}

TxValue::~TxValue()
{

}

void TxValue::setValue(const stk::StkFloat& value)
{
  _value = value;
}

stk::StkFloat TxValue::tick(unsigned int)
{
  return _value;
}

stk::StkFrames& TxValue::tick(stk::StkFrames& frames, unsigned int channel)
{
  for(size_t i = 0; i < frames.size(); ++i) {
    frames[i] = _value;
  }
  return frames;
}

void TxValue::_drawImpl(bool*)
{

}

void TxValue::reset()
{
  
}