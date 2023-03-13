#include "common.h"
#include "parameter.h"
#include "node.h"


// TxParameter (base class)
// ---------------------------------------------------------------
TxParameter::TxParameter(const std::string& name, void* data, short type)
  : _name(name)
  , _input(NULL)
  , _type(type)
  , _data(data)
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
TxParameterBool::TxParameterBool(const std::string& name, bool* data)
  : TxParameter(name, (void*)data, TxParameter::BOOL)
{
}
void TxParameterBool::set(stk::StkFloat value)
{
  *(bool*)_data = (bool)value;
}

stk::StkFloat TxParameterBool::tick()
{
  if(_input) {
    const bool value = (bool)_input->tick();
    return (stk::StkFloat)value;
  }
  return *(stk::StkFloat*)_data;
}

void TxParameterBool::draw()
{
  ImGui::Checkbox(_name.c_str(), (bool*)_data);
}

// TxParameterInt
// ---------------------------------------------------------------
TxParameterInt::TxParameterInt(const std::string& name, int minimum, int maximum, int* data)
  : TxParameter(name, (void*)data, TxParameter::INT)
  , _minimum(minimum)
  , _maximum(maximum)
{
}

void TxParameterInt::set(stk::StkFloat value)
{
  *(int*)_data = (int)value;
}

stk::StkFloat TxParameterInt::tick()
{
  if(_input) {
    const int value = (int)_input->tick();
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

void TxParameterInt::draw()
{
  ImGui::SliderInt(_name.c_str(), (int*)_data, _minimum, _maximum);
}

// TxParameterEnum
// ---------------------------------------------------------------
TxParameterEnum::TxParameterEnum(const std::string& name, const char** names, int num, int* data)
  : TxParameter(name, (void*)data, TxParameter::ENUM)
  , _names(names)
  , _num(num)
{
}

void TxParameterEnum::set(stk::StkFloat value)
{
  *(int*)_data = (int)value % _num;
}

stk::StkFloat TxParameterEnum::tick()
{
  if(_input) {
    const int value = ((int)_input->tick()) % _num;
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

void TxParameterEnum::draw()
{
  int value = *(int*)_data;
  if (ImGui::BeginCombo(_name.c_str(), _names[value], ImGuiComboFlags_NoArrowButton))
  {
    for (int n = 0; n < _num; ++n)
    {
      const bool is_selected = (value == n);
      if(ImGui::Selectable(_names[n], is_selected)) {
        *(int*)_data = n;
      }
      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
}

// TxParameterFloat
// ---------------------------------------------------------------
TxParameterFloat::TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum, 
  stk::StkFloat* data)
  : TxParameter(name, (void*)data, TxParameter::FLOAT)
  , _minimum(minimum)
  , _maximum(maximum)
{
}

void TxParameterFloat::set(stk::StkFloat value)
{
  *(stk::StkFloat*)_data = value;
}

stk::StkFloat TxParameterFloat::tick()
{
  if(_input) {
    
    return _input->tick();
  }
  return *(stk::StkFloat*)_data;
}

void TxParameterFloat::draw()
{
  ImGui::SliderFloat(_name.c_str(), (stk::StkFloat*)_data, _minimum, _maximum);
}

// TxParameterSamples
// ---------------------------------------------------------------
TxParameterSamples::TxParameterSamples(const std::string& name)
  : TxParameter(name, NULL)
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

void TxParameterSamples::draw()
{
  //ImGui::SliderInt(_name.c_str(), &_value, _minimum, _maximum);
}
