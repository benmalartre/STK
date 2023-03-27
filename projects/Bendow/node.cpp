#include "common.h" 
#include "node.h"
#include "graph.h"

const int TxNode::Flags = 
  ImGuiWindowFlags_NoBackground |
  ImGuiWindowFlags_NoScrollbar |
  ImGuiWindowFlags_NoBringToFrontOnFocus |
  ImGuiWindowFlags_ChildWindow ;

const int TxNode::PortSize = 32;

const char* TxNode::NodeName[TxNode::NumNode] = {
  "Value",
  "Random",
  "Oscillator",
  "Sequencer",
  "Lfo",
  "Adsr",
  "Arythmetic",
  "Filter",
  "Effect",
  "Mixer"
};

TxNode::TxNode(TxGraph* parent, const std::string& name, uint32_t numChannels)
  : _parent(parent)
  , _nChannels(numChannels)
  , _name(name)
  , _dirty(true)
  , _position(RANDOM_LO_HI(0,200), RANDOM_LO_HI(0,200))
  , _size(100,25)
  , _color(RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1.f)
{
  _parent->addNode(this);
  _frames.resize((int)stk::Stk::sampleRate(), 1, 0.0);
  _params.push_back(new TxParameterSamples(this, "Output", true, _nChannels));
}

TxNode::~TxNode()
{
  for(auto& param: _params){
    delete param;
  }
}

int TxNode::numChannels()
{
  return _nChannels;
}

int TxNode::pick(const ImVec2& pos)
{
  return -1;
}

bool TxNode::selected()
{
  return _selected;
}

const std::string& TxNode::name()
{
  return _name;
}

const ImVec2& TxNode::position()
{
  return _position;
}

const ImVec4& TxNode::color()
{
  return _color;
}

const ImColor& TxNode::color(short colorIdx)
{
  if (_selected) {
    switch (colorIdx) {
    case TX_COLOR_PLUG_ID:
      return TX_PLUG_COLOR_SELECTED;
    case TX_COLOR_CONTOUR_ID:
      return TX_CONTOUR_COLOR_SELECTED;
    }
  }
  else {
    switch (colorIdx) {
    case TX_COLOR_PLUG_ID:
      return TX_PLUG_COLOR_DEFAULT;
    case TX_COLOR_CONTOUR_ID:
      return TX_CONTOUR_COLOR_DEFAULT;
    }
  }
  return ImColor({ 100,100,100,255 });
}

TxGraph* TxNode::graph()
{
  return _parent;
}

void TxNode::setDirty(bool state) 
{
  _dirty = state;
};

void TxNode::setSelected(bool state)
{
  _selected = state;
};

void TxNode::setPosition(const ImVec2& pos)
{
  _position = pos;
}

stk::StkFloat TxNode::lastSample(unsigned int channel)
{
  return _frames[channel];
}

TxConnexion* TxNode::connect(TxNode* node, const std::string& name, short channel) 
{
  TxParameter* param = parameter(name);
  if(param) {
    param->connect(node, channel);
    std::cout << "connect : " << node->name() << " -> " << 
      _name << ":" << name << "(channel=" << channel << ")" << std::endl;
    TxConnexion* connexion = new TxConnexion({node->_params[OUTPUT], param, channel});
    _parent->addConnexion(connexion);
    return connexion;
  }
  return NULL;
}

void TxNode::disconnect(const std::string& name) 
{
  TxParameter* param = parameter(name);
  if(param) {
    param->disconnect();
  }
}

TxParameter* TxNode::parameter(const std::string& name)
{
  for(auto& param: _params) {
    if(param->name() == name) {
      return param;
    }
  }
  return NULL;
}

void TxNode::_drawOutput()
{
  _params[OUTPUT]->draw();
}

void TxNode::_drawAlignLeft()
{
  ImGui::SetCursorPosX((_position.x + TX_PLUG_WIDTH + TX_PADDING_X) * _parent->scale() + _parent->offset()[0]);
}

void TxNode::draw(bool* modified)
{
  const float scale = _parent->scale();
  const ImVec2 position = _position * scale + _parent->offset();

  //ImGui::PushStyleColor(ImGuiCol_ChildBg, _color);
  ImGui::SetCursorPos(position);
  //ImGui::BeginChild(_name.c_str(), size() * scale, true, TxNode::Flags);

  //ImGui::SetCursorPos(ImGui::GetCursorPos() + (ImVec2(TX_TITLE_X, TX_TITLE_Y)) * scale);
  //ImGui::PushFont()
  ImGui::SetWindowFontScale(1.0);
  ImGui::TextColored({ 255,255,255,255 }, _name.c_str());
  ImGui::SetWindowFontScale(0.5);
  _parent->setSplitterChannel(TxGraph::MIDDLE + _selected);
  _drawAlignLeft();
  _drawImpl(modified);

  //ImGui::EndChild();

  //ImGui::PopStyleColor();
  /*
  ImGui::SetCursorPos(position);
  ImGui::PushStyleColor(ImGuiCol_Button, _color);
  if (ImGui::Button("##hiddenBtn", size() * scale)) {
  }
  ImGui::PopStyleColor();
  */
  _parent->setSplitterChannel(TxGraph::FIRST + _selected);
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  drawList->AddRect(
    position + ImVec2(TX_PLUG_WIDTH, 0) * scale,
    position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, 
    color(TX_COLOR_CONTOUR_ID), 
    TX_NODE_ROUNDING * scale, 0, TX_CONTOUR_WIDTH * scale);
  
  drawList->AddRectFilled(
    position + ImVec2(TX_PLUG_WIDTH, 0) * scale, 
    position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, ImColor(_color), TX_NODE_ROUNDING * scale);
  
}