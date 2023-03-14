#include "common.h"
#include "parameter.h"
#include "node.h"


// TxParameter (base class)
// ---------------------------------------------------------------
TxParameter::TxParameter(const std::string& name, void* data, short type, short display)
  : _name(name)
  , _label(name)
  , _input(NULL)
  , _type(type)
  , _display(display)
  , _data(data)
  , _callback(NULL)
  , _channel(0)
{
}

TxParameter::~TxParameter()
{
  if(_callback){
    delete _callback;
  }
}

const std::string& TxParameter::name()
{
  return _name;
};

const std::string& TxParameter::label()
{
  return _label;
};

void TxParameter::setLabel(const std::string& label)
{
  _label = label;
}

void TxParameter::connect(TxNode* node, short channel)
{
  _input = node;
  _channel = channel;
}

void TxParameter::disconnect()
{
  _input = NULL;
}

void TxParameter::setCallback(TxCallback* callback)
{
  _callback = callback;
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
    const bool value = (bool)_input->tick(_channel);
    return (stk::StkFloat)value;
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterBool::draw()
{
  return ImGui::Checkbox(_label.c_str(), (bool*)_data);
}

// TxParameterInt
// ---------------------------------------------------------------
TxParameterInt::TxParameterInt(const std::string& name, int minimum, int maximum, 
  int* data, short display)
  : TxParameter(name, (void*)data, TxParameter::INT, display)
  , _minimum(minimum)
  , _maximum(maximum)
{
}

void TxParameterInt::set(stk::StkFloat value)
{
  *(int*)_data = (int)value;
}

void TxParameterInt::setMinimum(int value)
{
  _minimum = value;
}

void TxParameterInt::setMaximum(int value)
{
  _maximum = value;
}

stk::StkFloat TxParameterInt::tick()
{
  if(_input) {
    const int value = (int)_input->tick(_channel);
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

bool TxParameterInt::draw()
{
  bool modified = false;
  switch(_display) {
    case TxParameter::HORIZONTAL:
      modified = ImGui::SliderInt(_label.c_str(), (int*)_data, _minimum, _maximum);
      break;
    case TxParameter::VERTICAL:
      modified = ImGui::VSliderInt(_label.c_str(),ImVec2(20, 100), (int*)_data, _minimum, _maximum);
      break;
    case TxParameter::KNOB:
      modified = ImGuiKnobs::KnobInt(_label.c_str(), (int*)_data, _minimum, _maximum);
      break;
  }
  if(modified && _callback) _callback->execute();
  return modified;
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
    const int value = ((int)_input->tick(_channel)) % _num;
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

bool TxParameterEnum::draw()
{
  int value = *(int*)_data;
  bool modified = false;
  if (ImGui::BeginCombo(_label.c_str(), _names[value], ImGuiComboFlags_NoArrowButton))
  {
    for (int n = 0; n < _num; ++n)
    {
      const bool is_selected = (value == n);
      if(ImGui::Selectable(_names[n], is_selected)) {
        *(int*)_data = n;
        modified = true;
      }
      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  if(modified && _callback) _callback->execute();
  return modified;
}

// TxParameterFloat
// ---------------------------------------------------------------
TxParameterFloat::TxParameterFloat(const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum, 
  stk::StkFloat* data, short display)
  : TxParameter(name, (void*)data, TxParameter::FLOAT, display)
  , _minimum(minimum)
  , _maximum(maximum)
{
}

void TxParameterFloat::set(stk::StkFloat value)
{
  *(stk::StkFloat*)_data = value;
}

void TxParameterFloat::setMinimum(stk::StkFloat value)
{
  _minimum = value;
}

void TxParameterFloat::setMaximum(stk::StkFloat value)
{
  _maximum = value;
}

stk::StkFloat TxParameterFloat::tick()
{
  if(_input) {
    
    return _input->tick(_channel);
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterFloat::draw()
{
  bool modified = false;
  switch(_display) {
    case TxParameter::HORIZONTAL:
      modified = ImGui::SliderFloat(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum);
      break;
    case TxParameter::VERTICAL:
      modified = ImGui::VSliderFloat(_label.c_str(),ImVec2(20, 100), (stk::StkFloat*)_data, _minimum, _maximum);
      break;
    case TxParameter::KNOB:
      modified = ImGuiKnobs::Knob(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum, 
        0.f, "%.3f", ImGuiKnobVariant_WiperDot);
      break;
  }
  if(modified && _callback) _callback->execute();

  return modified;
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

bool TxParameterSamples::draw()
{
  return false;
}
