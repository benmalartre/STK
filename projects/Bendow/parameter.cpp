#include "common.h"
#include "parameter.h"
#include "node.h"
#include "graph.h"

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
TxParameter::TxParameter(TxNode* node, const std::string& name, void* data, short type, int flags)
  : _node(node)
  , _name(name)
  , _label(name)
  , _input(NULL)
  , _type(type)
  , _flags(flags)
  , _data(data)
  , _callback(NULL)
  , _iChannel(0)
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

void TxParameter::_drawPlug(short channel)
{
  TxGraph* graph = _node->graph();
  const float& scale = graph->scale();
  ImVec2 center;
  float radius;
  bool horizontal = (_type == TxParameter::BOOL) || _flags & TxParameter::HORIZONTAL;
  if (horizontal) {
    center = ImGui::GetCursorScreenPos() + ImVec2(8, 8) * scale;
    radius = 4.f * scale;
    ImGui::InvisibleButton("##plug", ImVec2(2 * radius, 2 * radius));
  } else {
    center = ImGui::GetCursorScreenPos() + ImVec2(24, 12) * scale;
    radius = 8.f * scale;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + radius);
    ImGui::InvisibleButton("##plug", ImVec2(2 * radius, 2 * radius));
  }

  ImColor plugColor = TX_PLUG_COLOR_DEFAULT;
  
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
    ImGui::Text("This is a drag and drop source");
    ImGui::EndDragDropSource();
  } else if (ImGui::IsDragDropActive()) {
    if (_checkCompatibility(this, graph->connexion()) && ImGui::BeginDragDropTarget()) {
      _node->graph()->updateConnexion(this, channel, true);
      plugColor = TX_PLUG_COLOR_SELECTED;
      ImGui::EndDragDropTarget();
    } else {
      _node->graph()->updateConnexion(this, 0, false);
    }
  }

  TxConnexion* connexion = _node->graph()->connexion();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  drawList->AddCircle(center, radius, TX_CONTOUR_COLOR_DEFAULT, 32, TX_CONTOUR_WIDTH * scale);
  drawList->AddCircleFilled(center, radius, TX_PLUG_COLOR_DEFAULT, 32);
  drawList->AddCircleFilled(center, radius * 0.6, TX_CONTOUR_COLOR_DEFAULT, 32);

  if (connexion && _checkCompatibility(this, graph->connexion())) {
    ImDrawList* foregroundList = ImGui::GetForegroundDrawList();
    foregroundList->AddCircle(center, radius, TX_PLUG_COLOR_AVAILABLE, 32, 2 * TX_CONTOUR_WIDTH * scale);
    //foregroundList->AddCircleFilled(center, radius, TX_PLUG_COLOR_AVAILABLE, 32);
  }
  
  _plug[channel] = center;
  _radius[channel] = radius;
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
    const bool value = (bool)_input->tick(_iChannel);
    return (stk::StkFloat)value;
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterBool::draw()
{
  ImGui::BeginGroup();
  _drawPlug(0);

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
    const int value = (int)_input->tick(_iChannel);
    return (stk::StkFloat)value;
  }
  return (stk::StkFloat)*(int*)_data;
}

namespace SevenSegments {
#define W 15
#define H 7.5
#define dr(n,i)d->AddConvexPolyFilled(pp,6,(kd[n]>>(6-i))&1 ? 0xffffffff : 0xff222222)
  char kd[] = { 0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B };

  void digit(ImDrawList* d, int n, ImVec2 e, ImVec2 p) {
    float r[7][4] = { 
      {-1,-1,H,H},
      {1,-1,-H,H},
      {1,0,-H,-H},
      {-1,1,H,-W * 1.5},
      {-1,0,H,-H},
      {-1,-1,H,H},
      {-1,0,H,-H}, 
    }; 
    for (int i = 0; i < 7; i++) {
      ImVec2 a, b; 
      if (i % 3 == 0) { 
        a = ImVec2(p.x + r[i][0] * e.x + r[i][2], p.y + r[i][1] * e.y + r[i][3] - H);
        b = ImVec2(a.x + e.x * 2 - W, a.y + W); 
      } else { 
        a = ImVec2(p.x + r[i][0] * e.x + r[i][2] - H, p.y + r[i][1] * e.y + r[i][3]);
        b = ImVec2(a.x + W, a.y + e.y - W); 
      }
      ImVec2 q = b - a; 
      float s = W * 0.6, u = s - H; 
      if (q.x > q.y) { 
        ImVec2 pp[] = { 
          {a.x + u,a.y + q.y * .5f},
          {a.x + s,a.y},
          {b.x - s,a.y},
          {b.x - u,a.y + q.y * .5f},
          {b.x - s,b.y},
          {a.x + s,b.y} 
        }; 
        dr(n, i); 
      } else { 
        ImVec2 pp[] = { 
          {a.x + q.x * .5f,a.y + u},
          {b.x,a.y + s},
          {b.x,b.y - s},
          {b.x - q.x * .5f,b.y - u},
          {a.x,b.y - s},
          {a.x,a.y + s} };
        dr(n, i); 
      } 
    } 
  }
  
  void FX(ImDrawList* d, ImVec2 a, ImVec2 b, ImVec2 sz, ImVec4, float t) 
  { 
    int _t = int(t / 60) * 100 + fmod(t, 60); 
    float p = 0.02 * sz.x, s = sz.x / 4 - p, x = s * .5, y = sz.y * .5; 
    for (int i = 0; i < 4; i++) { 
      int _d = _t % 10; _t /= 10; 
      digit(d, _d, ImVec2(s * .5, y), ImVec2(b.x - x, a.y + y)); x += s + p; 
    } 
  }
}
bool TxParameterInt::draw()
{
  bool modified = false;
  ImGui::BeginGroup();
  if (_flags & TxParameter::HORIZONTAL) {
    modified = ImGui::SliderInt(_label.c_str(), (int*)_data, _minimum, _maximum);
    ImGui::SameLine();
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderInt(_label.c_str(), ImVec2(20, 100) * _node->graph()->scale(), (int*)_data, _minimum, _maximum);
  } else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::KnobInt(_label.c_str(), (int*)_data, _minimum, _maximum, 0.f, 0, 1, TX_KNOB_SIZE);
  } else if (_flags & TxParameter::SEVENSEGMENTS) {

  }
  
  _drawPlug(0);
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
    const int value = ((int)_input->tick(_iChannel)) % _num;
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
    
    return _input->tick(_iChannel);
  }
  return *(stk::StkFloat*)_data;
}

bool TxParameterFloat::draw()
{
  bool modified = false;
  ImGui::BeginGroup();

  if (_flags & TxParameter::HORIZONTAL) {
    modified = ImGui::SliderFloat(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum);
    ImGui::SameLine();
  }
  else if (_flags & TxParameter::VERTICAL) {
    modified = ImGui::VSliderFloat(_label.c_str(), ImVec2(20, 100) * _node->graph()->scale(), (stk::StkFloat*)_data, _minimum, _maximum);
  }
  else if (_flags & TxParameter::KNOB) {
    modified = ImGuiKnobs::Knob(_label.c_str(), (stk::StkFloat*)_data, _minimum, _maximum,
      0.f, "%.3f", ImGuiKnobVariant_WiperDot, TX_KNOB_SIZE);
  }
  _drawPlug(0);

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
  : TxParameter(node, name, NULL, TxParameter::SAMPLES)
  , _frames(NULL)
  , _io(io)
  , _index(0)
  , _nChannels(nChannels)
{
}

void TxParameterSamples::setIndex(int index)
{
  _index = index;
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

bool TxParameterSamples::draw()
{
  ImGuiIO& io = ImGui::GetIO();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  TxGraph* graph = _node->graph();
  ImVec2 pMin, pMax;
  float plugOffsetY = 3 * TX_PLUG_HEIGHT;
  const float& scale = graph->scale();
  const ImVec2& offset = graph->offset();

  for (size_t c = 0; c < _nChannels; ++c) {
    if (!_io) {
      pMin = (_node->position() + ImVec2(0.f, TX_TITLE_HEIGHT + plugOffsetY * (c + _index))) * scale + offset;
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * scale;
    }
    else {
      pMin = (_node->position() + ImVec2(_node->size()[0] - TX_PLUG_WIDTH, TX_TITLE_HEIGHT + plugOffsetY * (c + _index))) * scale + offset;
      pMax = pMin + ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT) * scale;
    }

    ImGui::SetCursorPos((pMin - ImVec2(0, TX_PLUG_DETAIL * scale)) - ImGui::GetWindowPos());
    ImGui::InvisibleButton("##plug", ImVec2(TX_PLUG_WIDTH, TX_PLUG_HEIGHT + 2 * TX_PLUG_DETAIL) * scale);
    if (ImGui::BeginDragDropSource()) {
      _node->graph()->startConnexion(this, c);
      ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
      ImGui::Text("This is a drag and drop source");
      ImGui::EndDragDropSource();
    }
    else if (ImGui::IsDragDropActive()) {
      if (ImGui::BeginDragDropTarget()) {
        ImGui::Text("This is a drag and drop target");
        _node->graph()->updateConnexion(this, c, true);
        //ImGui::SetDragDropPayload(const char* type, const void* data, size_t data_size, ImGuiCond cond)
        ImGui::EndDragDropTarget();
      } else {
        _node->graph()->updateConnexion(this, 0, false);
      }
    }

    ImColor plugColor = _node->color(TX_COLOR_PLUG_ID);
    if (ImGui::IsDragDropActive() && _checkCompatibility(this, _node->graph()->connexion() )) {
      plugColor = TX_PLUG_COLOR_AVAILABLE;
    }

    drawList->AddRect(pMin, pMax, _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0, TX_CONTOUR_WIDTH * scale);
    if (!_io) {
      drawList->AddRect(
        pMin + ImVec2(TX_PLUG_WIDTH - TX_PLUG_DETAIL, -TX_PLUG_DETAIL) * scale,
        pMax + ImVec2(0, TX_PLUG_DETAIL) * graph->scale(),
        _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0,
        TX_CONTOUR_WIDTH * scale);
    }
    else {
      drawList->AddRect(
        pMin + ImVec2(0, -TX_PLUG_DETAIL) * graph->scale(),
        pMax + ImVec2(-TX_PLUG_WIDTH + TX_PLUG_DETAIL, TX_PLUG_DETAIL) * scale,
        _node->color(TX_COLOR_CONTOUR_ID), TX_NODE_ROUNDING * scale, 0,
        TX_CONTOUR_WIDTH * scale);
    }

    drawList->AddRectFilled(pMin, pMax, plugColor, TX_NODE_ROUNDING * scale);
    if (!_io) {
      drawList->AddRectFilled(
        pMin + ImVec2(TX_PLUG_WIDTH - TX_PLUG_DETAIL, -TX_PLUG_DETAIL) * scale,
        pMax + ImVec2(0, TX_PLUG_DETAIL) * graph->scale(),
        plugColor,
        TX_NODE_ROUNDING * scale);
    }
    else {
      drawList->AddRectFilled(
        pMin + ImVec2(0, -TX_PLUG_DETAIL) * graph->scale(),
        pMax + ImVec2(-TX_PLUG_WIDTH + TX_PLUG_DETAIL, TX_PLUG_DETAIL) * scale,
        plugColor,
        TX_NODE_ROUNDING * scale);
    }
    _plug[c] = (pMin + pMax) * 0.5;
    _radius[c] = std::sqrtf(ImLengthSqr(pMax - pMin));
  }
  return false;
}
