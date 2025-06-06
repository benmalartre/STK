#include "common.h"
#include "parameter.h"
#include "node.h"
#include "graph.h"
#include "editor.h"


static bool _checkCompatibility(TxParameter* param, TxConnexion* connexion)
{
  if (!connexion)return false;
  if (param->node() == connexion->source->node())return false;
  if (connexion->source->type() == TxParameter::SAMPLES && ((TxParameterSamples*)connexion->source)->io()) {
    return param->type() != TxParameter::SAMPLES || 
      ((TxParameterSamples*)connexion->source)->io() != ((TxParameterSamples*)param)->io();
  }
  else {
    return param->type() == TxParameter::SAMPLES && !((TxParameterSamples*)param)->io();
  }
}


// TxParameter (base class)
// ---------------------------------------------------------------
TxParameter::TxParameter(TxNode* node, const std::string& name, void* data, 
  size_t index, short type, int flags)
  : _node(node)
  , _name(name)
  , _label(name)
  , _index(index)
  , _input(NULL)
  , _type(type)
  , _flags(flags)
  , _data(data)
  , _callback(NULL)
  , _iChannel(0)
{
    _fullname = "##plug" + _node->name() +":"+_name + std::to_string(_index);
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

TxNode* TxParameter::input()
{
  return _input;
}

short TxParameter::type()
{
  return _type;
}

const int& TxParameter::flags()
{
  return _flags;
}

const std::string& TxParameter::name()
{
  return _name;
};


const std::string& TxParameter::label()
{
  return _label;
};

const ImVec2& TxParameter::plug(short channel)
{
  return _plug[channel];
}

const float& TxParameter::radius(short channel)
{
  return _radius[channel];
}

void TxParameter::setLabel(const std::string& label)
{
  _label = label;
}

void TxParameter::connect(TxNode* node, short channel)
{
  _input = node;
  _iChannel = channel;
}

void TxParameter::disconnect()
{
  _input = NULL;
}

void TxParameter::setCallback(TxCallback* callback)
{
  _callback = callback;
}

void TxParameter::drawPlug(TxEditor* editor, short index, short channel)
{
  const float scale = editor->scale();
  ImVec2 center;
  float radius;

  center = editor->pos() + (node()->position() + ImVec2(0, TX_PLUG_SIZE * (3 * index + 2))) * scale + editor->offset();
  radius = TX_PLUG_SIZE * scale;
  ImGui::SetCursorScreenPos(center - ImVec2(TX_PLUG_SIZE, TX_PLUG_SIZE) * scale);
  ImGui::Button(_fullname.c_str(), ImVec2(2 * radius, 2 * radius));

  ImColor plugColor = TX_PLUG_COLOR_DEFAULT;
  
  if (ImGui::IsDragDropActive()) {
    if (_checkCompatibility(this, editor->connexion()) && ImGui::BeginDragDropTarget()) {
      editor->updateConnexion(this, channel);
      plugColor = TX_PLUG_COLOR_SELECTED;
      ImGui::EndDragDropTarget();
    }
  } else if(_input && ImGui::BeginDragDropSource()) {
    TxConnexion* connexion = _input->connexion(this);
    if(connexion) {
      TxParameter* source = connexion->source;
      size_t channel = connexion->channel;
      
      node()->graph()->removeConnexion(connexion);
      
      _input = NULL;

      editor->startConnexion(source, channel);
      ImGui::SetDragDropPayload("disconnect", NULL, 0);
      std::string msg("disconnect "+node()->name()+":"+_name);
      ImGui::Text("%s", msg.c_str());
      ImGui::EndDragDropSource();
    }
  }

  TxConnexion* connexion = editor->connexion();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddText(ImVec2(center.x - TX_PLUG_SIZE * 2, center.y), TX_PLUG_COLOR_DEFAULT, _label.c_str());
  drawList->AddCircle(center, radius, TX_CONTOUR_COLOR_DEFAULT, 32, TX_CONTOUR_WIDTH * scale);
  drawList->AddCircleFilled(center, radius, TX_PLUG_COLOR_DEFAULT, 32);
  drawList->AddCircleFilled(center, radius * 0.6, TX_CONTOUR_COLOR_DEFAULT, 32);

  if (connexion && _checkCompatibility(this, editor->connexion())) {
    ImDrawList* foregroundList = ImGui::GetForegroundDrawList();
    foregroundList->AddCircle(center, radius, TX_PLUG_COLOR_AVAILABLE, 32, 2 * TX_CONTOUR_WIDTH * scale);
  }
  
  _plug[channel] = center;
  _radius[channel] = radius;
}

// TxParameterBool
// ---------------------------------------------------------------
TxParameterBool::TxParameterBool(TxNode* node, const std::string& name, bool* data, size_t index)
  : TxParameter(node, name, (void*)data, index, TxParameter::BOOL)
{
}
void TxParameterBool::set(stk::StkFloat value)
{
  *(bool*)_data = (bool)value;
}

stk::StkFloat TxParameterBool::tick()
{
  if(_input) {
    const bool value = (bool)_input->tick(_iChannel);
    return (stk::StkFloat)value;
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterBool::draw(TxEditor* editor)
{
  return ImGui::Checkbox(_name.c_str(), (bool*)_data);
}

// TxParameterInt
// ---------------------------------------------------------------
TxParameterInt::TxParameterInt(TxNode* node, const std::string& name, int minimum, int maximum,
  int* data, size_t index, int flags)
  : TxParameter(node, name, (void*)data, index, TxParameter::INT, flags)
  , _minimum(minimum)
  , _maximum(maximum)
  , _size(TX_KNOB_MIDDLE_SIZE)
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
    const int value = (int)_input->tick(_iChannel);
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

bool TxParameterInt::draw(TxEditor* editor)
{
  bool modified = false;
  ImGui::BeginGroup();
  if (_flags & TxParameter::HORIZONTAL) {
    modified = ImGui::SliderInt(_name.c_str(), (int*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderInt(_name.c_str(), ImVec2(20, 100) * editor->scale(), 
      (int*)_data, _minimum, _maximum);
  } else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::KnobInt(_name.c_str(), (int*)_data, 
      _minimum, _maximum, 0.f, 0, 1, _size * editor->scale());
  }
  
  ImGui::EndGroup();
  if(modified && _callback) _callback->execute();
  return modified;
}

// TxParameterEnum
// ---------------------------------------------------------------
TxParameterEnum::TxParameterEnum(TxNode* node, const std::string& name, 
  const char** names, int num, int* data, size_t index)
  : TxParameter(node, name, (void*)data, index, TxParameter::ENUM)
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
    const int value = ((int)_input->tick(_iChannel)) % _num;
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

bool TxParameterEnum::draw(TxEditor* editor)
{
  int value = *(int*)_data;
  bool modified = false;
  if (ImGui::BeginCombo(_name.c_str(), _names[value], ImGuiComboFlags_NoArrowButton))
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
  stk::StkFloat* data, size_t index, int flags)
  : TxParameter(node, name, (void*)data, index, TxParameter::FLOAT, flags)
  , _minimum(minimum)
  , _maximum(maximum)
  , _size(TX_KNOB_MIDDLE_SIZE)
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
    
    return _input->tick(_iChannel);
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterFloat::draw(TxEditor* editor)
{
  bool modified = false;
  ImGui::BeginGroup();

  if (_flags & TxParameter::HORIZONTAL) {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100 * editor->scale());
    modified = ImGui::SliderFloat(_name.c_str(), (float*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderFloat(_name.c_str(), ImVec2(20, 100) * editor->scale(), 
      (float*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::Knob(_name.c_str(), (float*)_data, _minimum, _maximum,
      0.f, "%.3f", ImGuiKnobVariant_WiperDot, _size * editor->scale());
  }
  
  ImGui::EndGroup();

  if(modified && _callback) _callback->execute();
  return modified;
}

// TxParameterString
// ---------------------------------------------------------------
TxParameterString::TxParameterString(TxNode* node, const std::string& name, std::string* data, 
  size_t index, int flags)
  : TxParameter(node, name, (void*)data, index, TxParameter::STRING, flags)
{
}

void TxParameterString::set(stk::StkFloat value)
{
}

stk::StkFloat TxParameterString::tick()
{
  return 0.f;
}

bool TxParameterString::draw(TxEditor* editor)
{
  bool modified = false;
  /*
  switch(_display) {
    case TxParameter::HORIZONTAL:
      modified = ImGui::SliderFloat(_name.c_str(), (stk::StkFloat*)_data, _minimum, _maximum);
      break;
    case TxParameter::VERTICAL:
      modified = ImGui::VSliderFloat(_name.c_str(),ImVec2(20, 100), (stk::StkFloat*)_data, _minimum, _maximum);
      break;
    case TxParameter::KNOB:
      modified = ImGuiKnobs::Knob(_name.c_str(), (stk::StkFloat*)_data, _minimum, _maximum, 
        0.f, "%.3f", ImGuiKnobVariant_WiperDot);
      break;
  }
  if(modified && _callback) _callback->execute();
  */

  return modified;
}

// TxParameterSamples
// ---------------------------------------------------------------
TxParameterSamples::TxParameterSamples(TxNode* node, const std::string& name, size_t index, bool io,  int nChannels)
  : TxParameter(node, name, NULL, index, TxParameter::SAMPLES)
  , _frames(NULL)
  , _io(io)
  , _nChannels(nChannels)
{
}

void TxParameterSamples::set(stk::StkFloat value)
{
}

bool TxParameterSamples::io()
{
  return _io;
}

stk::StkFloat TxParameterSamples::tick()
{
  if(_input) {
    
    return _input->tick(_iChannel);
  }
  return 0.f;
}

bool TxParameterSamples::draw(TxEditor* editor, const ImVec2& pMin, const ImVec2& pMax, float scale, short channel)
{
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  bool connected = false;
  std::string name = _fullname + std::to_string(channel) + "Btn";
  ImGui::SetCursorPos((pMin - ImVec2(0, TX_PLUG_DETAIL * scale)) - ImGui::GetWindowPos());
  ImGui::InvisibleButton(name.c_str(), 
    ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT + 2 * TX_PLUG_DETAIL) * scale);

  
  if (!ImGui::IsDragDropActive() && ImGui::BeginDragDropSource()) {
    if(!_io && input()) {
      TxConnexion* connexion = input()->connexion(this);
      TxParameter* source = connexion->source;
      size_t channel = connexion->channel;
      
      node()->graph()->removeConnexion(connexion);
      
      _input = NULL;

      editor->startConnexion(source, channel);
      ImGui::SetDragDropPayload("disconnect", NULL, 0);
      std::string msg("disconnect "+node()->name()+":"+_name);
      ImGui::Text("%s", msg.c_str());
      ImGui::EndDragDropSource();
    } else {
      editor->startConnexion(this, channel);
      ImGui::SetDragDropPayload("connect", NULL, 0);
      ImGui::EndDragDropSource();
    }
  } else {
    if (_checkCompatibility(this, editor->connexion()) && ImGui::BeginDragDropTarget()) {
      editor->updateConnexion(this, channel);
      ImGui::EndDragDropTarget();
      connected = true;
    }
  }
 

  ImColor plugColor = _node->color(TX_COLOR_PLUG_ID);

  drawList->AddRect(pMin, pMax, _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0, TX_CONTOUR_WIDTH * scale);
  if (!_io) {
    drawList->AddRect(
      pMin + ImVec2(TX_PLUG_WIDTH - TX_PLUG_DETAIL, -TX_PLUG_DETAIL) * scale,
      pMax + ImVec2(0, TX_PLUG_DETAIL) * editor->scale(),
      _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0,
      TX_CONTOUR_WIDTH * scale);
  }
  else {
    drawList->AddRect(
      pMin + ImVec2(0, -TX_PLUG_DETAIL) * editor->scale(),
      pMax + ImVec2(-TX_PLUG_WIDTH + TX_PLUG_DETAIL, TX_PLUG_DETAIL) * scale,
      _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0,
      TX_CONTOUR_WIDTH * scale);
  }

  drawList->AddRectFilled(pMin, pMax, plugColor, TX_NODE_ROUNDING * scale);
  if (!_io) {
    drawList->AddRectFilled(
      pMin + ImVec2(TX_PLUG_WIDTH - TX_PLUG_DETAIL, -TX_PLUG_DETAIL) * scale,
      pMax + ImVec2(0, TX_PLUG_DETAIL) * editor->scale(),
      plugColor,
      TX_NODE_ROUNDING * scale);
  }
  else {
    drawList->AddRectFilled(
      pMin + ImVec2(0, -TX_PLUG_DETAIL) * editor->scale(),
      pMax + ImVec2(-TX_PLUG_WIDTH + TX_PLUG_DETAIL, TX_PLUG_DETAIL) * scale,
      plugColor,
      TX_NODE_ROUNDING * scale);
  }
  _plug[channel] = (pMin + pMax) * 0.5;
  _radius[channel] = sqrtf(ImLengthSqr(pMax - pMin));
  return connected;
}


bool TxParameterSamples::draw(TxEditor* editor)
{
  ImGuiIO& io = ImGui::GetIO();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  TxGraph* graph = _node->graph();
  ImVec2 pMin, pMax;
  float plugOffsetY = 3 * TX_PLUG_HEIGHT;
  const float& scale = editor->scale();
  const ImVec2& offset = editor->offset();
  const ImVec2 ep = editor->pos();

  for (size_t channel = 0; channel < _nChannels; ++channel) {
    if (!_io) {
      pMin = ep + (_node->position() + ImVec2(0.f, TX_TITLE_HEIGHT + plugOffsetY * (channel + _index - TxNode::LAST))) * scale + offset;
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * scale;
    }
    else {
      pMin = ep + (_node->position() + ImVec2(_node->size()[0] - TX_PLUG_WIDTH, TX_TITLE_HEIGHT + plugOffsetY * (channel + _index))) * scale + offset;
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * scale;
    }

    draw(editor, pMin, pMax, scale, channel);

  }
  return false;
}
