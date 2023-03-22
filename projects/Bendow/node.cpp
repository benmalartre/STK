#include "common.h" 
#include "node.h"

const int TxNode::Flags = 
  ImGuiWindowFlags_NoScrollbar |
  ImGuiWindowFlags_NoBringToFrontOnFocus;

const int TxNode::PortSize = 32;

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _nChannels(numChannels)
  , _name(name)
  , _dirty(true)
  , _position(RANDOM_LO_HI(0,200), RANDOM_LO_HI(0,200))
  , _size(100,25)
  , _color(RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1.0)
{
  _frames.resize((int)stk::Stk::sampleRate(), 1, 0.0);
  _params.push_back(new TxParameterSamples("Samples"));
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
    return new TxConnexion({node, this, parameter});
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

void TxNode::commonControls()
{
  ImGui::BeginGroup();
  _buffer.draw();
  ImGui::EndGroup();
}

void TxNode::draw(const ImVec2& offset, const float& scale, bool* modified)
{
  ImGui::PushStyleColor(ImGuiCol_ChildBg, _color);
  ImGui::SetCursorPos(_position * scale + offset);
  ImGui::BeginChild(_name.c_str(), size() * scale, NULL, TxNode::Flags);

  ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 10 , ImGui::GetCursorPosY() + 10) * scale);
  _drawImpl(modified);

  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void TxNode::drawInput()
{
  ImGui::BeginGroup();
  ImGui::Selectable("Input", false, 0, ImVec2(TxNode::PortSize, TxNode::PortSize));
  ImGui::EndGroup();
  ImGui::SameLine();
}