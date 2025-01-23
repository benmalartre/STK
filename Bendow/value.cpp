#include "value.h"

ImVec2 TxValue::Size(200, 120);

TxValue::TxValue(TxNode* parent, const std::string& name)
  : TxNode(parent, TxNode::VALUE, name)
{
  _value = 0.f;
  _params.push_back(new TxParameterFloat(this, "Value", -10000, 10000, 
    &_value, VALUE, TxParameter::KNOB));
}

TxValue::~TxValue()
{

}

const ImVec2& TxValue::size()
{
  return TxValue::Size;
}

void TxValue::setValue(const stk::StkFloat& value)
{
  _value = value;
}

stk::StkFloat TxValue::tick(unsigned int)
{
  return _value;
}

void TxValue::_drawImpl(TxEditor* editor, bool* modified)
{
  ImGui::BeginGroup();
  TxParameter* value = _params[TxValue::VALUE];
  if (value->draw(editor) && modified)*modified = true;
  ImGui::EndGroup();

  ImGui::SameLine();
  TxNode::_drawOutput(editor);
}

void TxValue::reset()
{
  
}