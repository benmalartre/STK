#include "parameter.h"
#include "node.h"


// TxParameter (base class)
// ---------------------------------------------------------------
TxParameter::TxParameter(const std::string& name, TxNode* input)
  : _name(name)
  , _input(input)
{
}

const std::string& TxParameter::name()
{
  return _name;
};

void TxParameter::connect(TxNode* node)
{
  _input = node;
}

void TxParameter::disconnect()
{
  _input = NULL;
}

// TxParameterBool
// ---------------------------------------------------------------
TxParameterBool::TxParameterBool(const std::string& name, bool value, TxNode* input)
  : TxParameter(name, input)
  , _value(value)
{
}
void TxParameterBool::set(stk::StkFloat value)
{
  _value = (bool)value;
}

stk::StkFloat TxParameterBool::tick()
{
  if(_input) {
    const bool value = (bool)_input->tick();
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)_value;
}

// TxParameterInt
// ---------------------------------------------------------------
TxParameterInt::TxParameterInt(const std::string& name, int minimum, int maximum, int value, TxNode* input)
  : TxParameter(name, input)
  , _minimum(minimum)
  , _maximum(maximum)
  , _value(value)
{
}

void TxParameterInt::set(stk::StkFloat value)
{
  _value = (int)value;
}

stk::StkFloat TxParameterInt::tick()
{
  if(_input) {
    const int value = (int)_input->tick();
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)_value;
}

// TxParameterFloat
// ---------------------------------------------------------------
TxParameterFloat::TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum, 
  stk::StkFloat value, TxNode* input)
  : TxParameter(name, input)
  , _minimum(minimum)
  , _maximum(maximum)
  , _value(value)
{
}

void TxParameterFloat::set(stk::StkFloat value)
{
  _value = value;
}

stk::StkFloat TxParameterFloat::tick()
{
  if(_input) {
    
    return _input->tick();
  }
  return (stk::StkFloat)_value;
}

// TxParameterSamples
// ---------------------------------------------------------------
TxParameterSamples::TxParameterSamples(const std::string& name, TxNode* input)
  : TxParameter(name, input)
  , _frames(NULL)
{
}

void TxParameterSamples::set(stk::StkFloat value)
{
}

stk::StkFloat TxParameterSamples::tick()
{
  return 0.f;
}
