#include "common.h"
#include "parameter.h"
#include "node.h"
#include "graph.h"


// TxParameter (base class)
// ---------------------------------------------------------------
TxParameter::TxParameter(TxNode* node, const std::string& name, void* data, short type, int flags)
  : _node(node)
  , _name(name)
  , _label(name)
  , _input(NULL)
  , _type(type)
  , _flags(flags)
  , _data(data)
  , _callback(NULL)
  , _channel(0)
  , _plug(0, 0)
{
}

TxParameter::~TxParameter()
{
  if(_callback){
    delete _callback;
  }
}

TxNode* TxParameter::node()
{
  return _node;
}
const std::string& TxParameter::name()
{
  return _name;
};

const std::string& TxParameter::label()
{
  return _label;
};

const ImVec2& TxParameter::plug()
{
  return _plug;
}

const float& TxParameter::radius()
{
  return _radius;
}

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

void TxParameter::_drawPlug()
{
  ImGuiIO& io = ImGui::GetIO();
  const float scale = io.FontGlobalScale;
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  const ImVec2 center = ImGui::GetCursorScreenPos() + ImVec2(24, 12) * scale;
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12);
  ImGui::InvisibleButton("##plug", ImVec2(24 * scale, 24 * scale));
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
    ImGui::Text("This is a drag and drop source");
    ImGui::EndDragDropSource();
  }
  else if (ImGui::IsDragDropActive()) {
    if (ImGui::BeginDragDropTarget()) {
      ImGui::Text("This is a drag and drop target");
      ImGui::EndDragDropTarget();
    }
  }

  drawList->AddCircleFilled(center, 8.f * scale, ImColor({ 0,0,0,128 }), 32);
  drawList->AddCircle(center, 12.f * scale, ImColor({ 180,180,180,255 }), 32, 4.f * scale);

  _plug = center;
  _radius = 16.f * scale;
}

// TxParameterBool
// ---------------------------------------------------------------
TxParameterBool::TxParameterBool(TxNode* node, const std::string& name, bool* data)
  : TxParameter(node, name, (void*)data, TxParameter::BOOL)
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
  ImGui::BeginGroup();
  _drawPlug();

  ImGui::SameLine();
  bool modified = ImGui::Checkbox(_label.c_str(), (bool*)_data);
  ImGui::EndGroup();
  return modified;
}

// TxParameterInt
// ---------------------------------------------------------------
TxParameterInt::TxParameterInt(TxNode* node, const std::string& name, int minimum, int maximum,
  int* data, int flags)
  : TxParameter(node, name, (void*)data, TxParameter::INT, flags)
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
  ImGui::BeginGroup();
  if (_flags & TxParameter::HORIZONTAL) {
    modified = ImGui::SliderInt(_label.c_str(), (int*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderInt(_label.c_str(), ImVec2(20, 100), (int*)_data, _minimum, _maximum);
  } else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::KnobInt(_label.c_str(), (int*)_data, _minimum, _maximum);
  }
  
  _drawPlug();
  ImGui::EndGroup();
  if(modified && _callback) _callback->execute();
  return modified;
}

// TxParameterEnum
// ---------------------------------------------------------------
TxParameterEnum::TxParameterEnum(TxNode* node, const std::string& name, const char** names, int num, int* data)
  : TxParameter(node, name, (void*)data, TxParameter::ENUM)
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
TxParameterFloat::TxParameterFloat(TxNode* node, const std::string& name, stk::StkFloat minimum, stk::StkFloat maximum,
  stk::StkFloat* data, int flags)
  : TxParameter(node, name, (void*)data, TxParameter::FLOAT, flags)
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
  ImGui::BeginGroup();
  if (_flags & TxParameter::HORIZONTAL) {
    modified = ImGui::SliderFloat(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderFloat(_label.c_str(), ImVec2(20, 100), (stk::StkFloat*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::Knob(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum,
      0.f, "%.3f", ImGuiKnobVariant_WiperDot);
  }

  _drawPlug();

  ImGui::EndGroup();

  if(modified && _callback) _callback->execute();
  return modified;
}

// TxParameterString
// ---------------------------------------------------------------
TxParameterString::TxParameterString(TxNode* node, const std::string& name, std::string* data, int flags)
  : TxParameter(node, name, (void*)data, TxParameter::STRING, flags)
{
}

void TxParameterString::set(stk::StkFloat value)
{
}

stk::StkFloat TxParameterString::tick()
{
  return 0.f;
}

bool TxParameterString::draw()
{
  bool modified = false;
  /*
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
  */

  return modified;
}

// TxParameterSamples
// ---------------------------------------------------------------
TxParameterSamples::TxParameterSamples(TxNode* node, const std::string& name, bool io, int nChannels)
  : TxParameter(node, name, NULL)
  , _frames(NULL)
  , _io(io)
  , _nChannels(nChannels)
{
}

void TxParameterSamples::set(stk::StkFloat value)
{
}

stk::StkFloat TxParameterSamples::tick()
{
  if(_input) {
    
    return _input->tick(_channel);
  }
  return 0.f;
}

stk::StkFloat TxParameterSamples::tick(unsigned int channel)
{
  if (_input) {

    return _input->tick(channel);
  }
  return 0.f;
}

bool TxParameterSamples::draw()
{
  ImGuiIO& io = ImGui::GetIO();
  ImDrawList* drawList = ImGui::GetForegroundDrawList();
  TxGraph* graph = _node->graph();
  ImVec2 pMin, pMax;
  float plugOffsetY = _node->size()[1] / (_nChannels + 1);

  for (size_t c = 0; c < _nChannels; ++c) {
    if (!_io) {
      pMin = (_node->position() + ImVec2(0.f, plugOffsetY * (c+1))) * graph->scale() + graph->offset();
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * graph->scale();
    }
    else {
      pMin = (_node->position() + ImVec2(_node->size()[0] - TX_PLUG_WIDTH, plugOffsetY * (c + 1))) * graph->scale() + graph->offset();
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * graph->scale();
    }

    ImGui::SetCursorPos(pMin - ImGui::GetWindowPos());
    ImGui::Button("##plug", ImVec2(TX_PLUG_WIDTH * 2, TX_PLUG_HEIGHT * 2) * graph->scale());
    if (ImGui::BeginDragDropSource()) {
      ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
      ImGui::Text("This is a drag and drop source");
      ImGui::EndDragDropSource();
    }
    else if (ImGui::IsDragDropActive()) {
      if (ImGui::BeginDragDropTarget()) {
        ImGui::Text("This is a drag and drop target");
        ImGui::EndDragDropTarget();
      }
    }

    drawList->AddRectFilled(pMin, pMax, ImColor({ 180,180,180,255 }), 2.f * graph->scale());
    if (!_io) {
      drawList->AddRectFilled(
        pMin + ImVec2(TX_PLUG_WIDTH - TX_PLUG_DETAIL, -TX_PLUG_DETAIL) * graph->scale(),
        pMax + ImVec2(0, TX_PLUG_DETAIL) * graph->scale(),
        ImColor({ 180,180,180,255 }),
        2.f * graph->scale());
    }
    else {
      drawList->AddRectFilled(
        pMin + ImVec2(0, -TX_PLUG_DETAIL) * graph->scale(),
        pMax + ImVec2(-TX_PLUG_WIDTH + TX_PLUG_DETAIL, TX_PLUG_DETAIL) * graph->scale(),
        ImColor({ 180,180,180,255 }),
        2.f * graph->scale());
    }
  }

  _plug = (pMin + pMax) * 0.5;
  _radius = std::sqrtf(ImLengthSqr(pMax - pMin));
  return false;
}
