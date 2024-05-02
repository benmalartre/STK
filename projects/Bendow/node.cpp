#include "common.h" 
#include "node.h"
#include "graph.h"
#include "track.h"
#include "editor.h"

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

TxNode::TxNode(TxNode* parent, short type, const std::string& name, uint32_t numChannels)
  : _parent(parent)
  , _type(type)
  , _nChannels(numChannels)
  , _name(name)
  , _dirty(true)
  , _active(true)
  , _position(RANDOM_LO_HI(0,200), RANDOM_LO_HI(0,200))
  , _size(100,25)
  , _color(RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1.f)
{
  if(_parent && _parent->type() == TxNode::GRAPH) ((TxGraph*)_parent)->addNode(this);
  _frames.resize((int)stk::Stk::sampleRate(), 1, 0.0);
  _params.push_back(new TxParameterBool(this, "Active", &_active));
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

short TxNode::type()
{
  return _type;
}

const std::string& TxNode::name()
{
  return _name;
}

const ImVec2& TxNode::position()
{
  return _position;
}

const ImU32 TxNode::color()
{
  return IM_COL32(_color.x, _color.y, _color.z, _color.w);
}

const ImU32 TxNode::color(short colorIdx)
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
  return IM_COL32(100,100,100,255 );
}

TxNode* TxNode::parent()
{
  return _parent;
}

TxGraph* TxNode::graph()
{
  if(_type == TxNode::GRAPH)return (TxGraph*)this;
  else if(_type == TxNode::TRACK) return ((TxTrack*)this)->graph();
  else if(_parent)
    if(_parent->type() == TxNode::GRAPH)return (TxGraph*)_parent;
    else return _parent->graph();
  else return NULL;
}

TxTrack* TxNode::track()
{
  if(_parent)
    if(_parent->type() == TxNode::TRACK)return (TxTrack*)_parent;
    else return _parent->track();
  else return NULL;
  
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
  TxParameter* param = node->parameter(name);
  if(param) {
    param->connect(this, channel);
    std::cout << "connected : " << _name << " -> " << 
      node->name() << ":" << name << "(channel=" << channel << ")" << std::endl;
    TxConnexion* connexion = new TxConnexion({_params[OUTPUT], param, channel});
    std::cout << "GRAPH : " << graph() << std::endl;
    graph()->addConnexion(connexion);
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

void TxNode::_drawPopup(TxEditor* editor)
{

  if (ImGui::BeginPopup((_name + "Popup").c_str())) {
    //ImGui::ColorButton("Color", _color);
    ImGui::ColorEdit3("Color", &_color.x);

    // State
    static char input[32]{ "" };

    // Code
    const bool is_input_text_enter_pressed = ImGui::InputText("##input", input, sizeof(input), ImGuiInputTextFlags_EnterReturnsTrue);
    const bool is_input_text_active = ImGui::IsItemActive();
    const bool is_input_text_activated = ImGui::IsItemActivated();

    if (is_input_text_activated)
      ImGui::OpenPopup("##popup");

    {
      ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
      //ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });
      if (ImGui::BeginPopup("##popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
      {
        static const char* autocomplete[] = { "cats", "dogs", "rabbits", "turtles" };

        for (int i = 0; i < IM_ARRAYSIZE(autocomplete); i++)
        {
          //if (strstr(autocomplete[i], input) == NULL)
          //    continue;
          if (ImGui::Selectable(autocomplete[i]))
          {
            ImGui::ClearActiveID();
            strcpy(input, autocomplete[i]);
          }
        }

        if (is_input_text_enter_pressed || (!is_input_text_active && !ImGui::IsWindowFocused()))
          ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
      }
    }

    ImGui::EndPopup();
  }
}

void TxNode::_drawOutput(TxEditor* editor)
{
  _params[OUTPUT]->draw(editor);
}

void TxNode::_drawAlignLeft(TxEditor* editor)
{
  const ImVec2& pos = editor->pos();
  ImGui::SetCursorPosX((_position.x + TX_PLUG_WIDTH + TX_PADDING_X) * editor->scale() + editor->offset()[0]);
}

void TxNode::_drawAlignTop(TxEditor* editor)
{
  const ImVec2& pos = editor->pos();
  ImGui::SetCursorPosY((_position.y + TX_PADDING_Y) * editor->scale() + editor->offset()[1]);
}

void TxNode::draw(TxEditor* editor, bool* modified)
{
  const float scale = editor->scale();

  const ImVec2 offset = editor->pos();
  const ImVec2 position = (_position)* scale + editor->offset();

  editor->setSplitterChannel(TxEditor::FOREGROUND);
  //ImGui::SetCursorPos(position);
  _drawAlignLeft(editor);
  _drawAlignTop(editor);
  _drawImpl(editor, modified);

  editor->setSplitterChannel(TxEditor::BACKGROUND);
  
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  // draw node name
  ImGui::PushFont(TX_FONT_TITLE);
  drawList->AddText(
    offset + position + ImVec2(TX_TITLE_X, -TX_TITLE_HEIGHT) * scale, 
    TX_CONTOUR_COLOR_SELECTED, 
    _name.c_str());
  ImGui::PopFont();

  // draw node contour
  drawList->AddRect(
    offset +position + ImVec2(TX_PLUG_WIDTH, 0) * scale,
    offset + position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, 
    color(TX_COLOR_CONTOUR_ID), 
    TX_NODE_ROUNDING * scale, 0, TX_CONTOUR_WIDTH * scale);
  
  // draw node background
  drawList->AddRectFilled(
    offset + position + ImVec2(TX_PLUG_WIDTH, 0) * scale, 
    offset + position + (size() - ImVec2(TX_PLUG_WIDTH, 0)) * scale, 
    ImColor(_color), 
    TX_NODE_ROUNDING * scale);
    
  _drawPopup(editor); 
}