#include "common.h" 
#include "node.h"
#include "graph.h"

const int TxNode::Flags = 
  ImGuiWindowFlags_NoBackground |
  ImGuiWindowFlags_NoScrollbar |
  ImGuiWindowFlags_NoBringToFrontOnFocus;

const int TxNode::PortSize = 32;

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

TxGraph* TxNode::graph()
{
  return _parent;
}

void TxNode::setDirty(bool state) 
{
  _dirty = state;
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
  TxParameter* parameter = getParameter(name);
  if(parameter) {
    parameter->connect(node, channel);
    std::cout << "connect : " << node->name() << " -> " << 
      _name << ":" << name << "(channel=" << channel << ")" << std::endl;
    TxConnexion* connexion = new TxConnexion({node->_params[OUTPUT], parameter, channel});
    _parent->addConnexion(connexion);
    return connexion;
  }
  return NULL;
}

void TxNode::disconnect(const std::string& name) 
{
  TxParameter* parameter = getParameter(name);
  if(parameter) {
    parameter->disconnect();
  }
}

TxParameter* TxNode::getParameter(const std::string& name)
{
  for(auto& param: _params) {
    if(param->name() == name) {
      return param;
    }
  }
  return NULL;
}

void TxNode::_drawCommonControls()
{
  ImGui::SameLine();
  ImGui::BeginGroup();
  _params[OUTPUT]->draw();
  ImGui::EndGroup();
}

void TxNode::_drawAlignLeft()
{
  ImGui::SetCursorPosX((TX_PLUG_WIDTH + TX_PADDING_X) * _parent->scale());
}

void TxNode::draw(bool selected, bool* modified)
{
  static const ImColor whiteColor({ 255,255,255,255 });
  const float scale = _parent->scale();
  const ImVec2 position = _position * scale + _parent->offset();
  ImGui::SetCursorPos(position);
  ImGui::BeginChild(_name.c_str(), size() * scale, NULL, TxNode::Flags);

  ImGui::SetCursorPos(ImGui::GetCursorPos() + (ImVec2(TX_TITLE_X, TX_TITLE_Y)) * scale);
  ImGui::TextColored({ 255,255,255,255 }, _name.c_str());
  _drawAlignLeft();
  _drawImpl(modified);

  ImGui::EndChild();

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  if (selected) {
    drawList->AddRect(
      position + ImVec2(TX_PLUG_WIDTH, 0) * scale,
      position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, whiteColor, 4.f * scale, 0, 8.f * scale);
  }
  drawList->AddRectFilled(
    position + ImVec2(TX_PLUG_WIDTH, 0) * scale, 
    position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, ImColor(_color), 4.f * scale);
}