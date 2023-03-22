#include "graph.h"

const int TxGraph::Flags = 
  ImGuiWindowFlags_NoResize | 
  ImGuiWindowFlags_NoCollapse |
  ImGuiWindowFlags_NoMove |
  ImGuiWindowFlags_NoNav |
  //ImGuiWindowFlags_NoBackground |
  ImGuiWindowFlags_NoTitleBar |
  ImGuiWindowFlags_NoInputs;

TxGraph::TxGraph(const std::string& name) 
  : _name(name)
  , _current(NULL)
  , _selected(NULL)
  , _active(true)
  , _offset(ImVec2(100,100))
  , _scale(1.f)
{};

TxGraph::~TxGraph()
{
  for(auto& node: _nodes)delete node;
};

bool TxGraph::contains(const TxNode* node)
{
  for(auto& n: _nodes) {
    if(node == n)return true;
  }
  return false;
}

int TxGraph::index(const TxNode* node)
{
  for(size_t i = 0; i < _nodes.size(); ++i) {
    if(_nodes[i] == node)return i;
  }
  return -1;
}

void TxGraph::setActive(bool state)
{
  _active = state;
}

void TxGraph::setCurrent(TxNode* node)
{
  _current = node;
}

TxNode* TxGraph::current()
{
  return _current;
}

TxNode* TxGraph::selected()
{
  return _selected;
}


std::vector<TxNode*>& TxGraph::nodes()
{
    return _nodes;
}

void TxGraph::addNode(TxNode* node) 
{
  if(!contains(node)) {
    _nodes.push_back(node);
    _current = node;
  }
}

void TxGraph::removeNode(TxNode* node) 
{
  int idx = index(node);
  if(idx >= 0) {
    _nodes.erase(_nodes.begin() + idx);
    delete node;
  }
}

stk::StkFloat TxGraph::tick()
{
  if(_current) return _current->tick();
  return 0.f;
}

static bool inside(const ImVec2& point, const ImVec2& bmin, const ImVec2& bmax)
{
  std::cout << "check inside: (" << point[0] << "," << point[1] << ") : (" << bmin[0] << "," << bmin[1] << "),(" << bmax[0] << ", " << bmax[1] << ")" << std::endl;
  return point[0] >= bmin[0] && point[0] <= bmax[0] &&
    point[1] >= bmin[1] && point[1] <= bmax[1];
}

int TxGraph::pick(const ImVec2& pos)
{
  _selected = NULL;
  size_t nodeIdx = _nodes.size()-1;
  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    std::cout << "check inside node : " << node->name() << std::endl;
    if(inside(pos, node->position() * _scale + _offset, 
      (node->position() + node->size()) * _scale + _offset))
      {
        _selected = node;
        std::cout << "selected node : " << _selected << std::endl;
        
        int portIdx = node->pick(pos);
        if(portIdx > -1) {
          return TxGraph::INPUT;
        }
        return TxGraph::NODE;
      }
  }
  return TxGraph::NONE;
}

void TxGraph::draw()
{
  static int counter = 0;
  static ImU32 whiteColor = ImColor({ 255,255,255,255 });
  static ImU32 blackColor = ImColor({ 0,0,0,255 });
  static ImVec2 offset;
  ImGuiIO& io = ImGui::GetIO();
  const ImVec2 pos(0, 100);
  const ImVec2 size = io.DisplaySize - pos;
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(size);
  ImGui::Begin(_name.c_str(), NULL, TxGraph::Flags);
  ImGui::SetWindowFontScale(_scale);
 
  int picked = TxGraph::NONE;
  if(io.MouseWheel) {
    _scale += io.MouseWheel * 0.1f;
  }
  if (io.MouseClicked[0]) {
    picked = pick(ImGui::GetMousePos());
    if (_selected) {
      offset = ImGui::GetMousePos() - _selected->position();
      if (_selected != _nodes[_nodes.size()-1]) {
        std::swap(_nodes[index(_selected)], _nodes[_nodes.size() - 1]);
        std::cout << "fuckinswapnodes :(" << std::endl;
      }
    }
  }

  bool modified = false;
  for (auto& node: _nodes) {
    node->draw(_offset, _scale, &modified);
  }

  if (!picked) {
    if (io.MouseDown[0] && _selected) {
      if (_selected) {
        _selected->setPosition(ImGui::GetMousePos() - offset);
      }
    }
    else if (io.MouseDown[2]) {
      _offset += io.MouseDelta;
      std::cout << "offset : " << _offset[0] << "," << _offset[1] << std::endl;
    }
  }
  
  //ImGui::PopClipRect();
  /*
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  ImGuiIO& io = ImGui::GetIO();
  if (io.MouseClicked[0] && inside(ImGui::GetMousePos(), ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(1024,1024))) {
    pick(io.MouseClickedPos[0] - ImGui::GetWindowPos());
  }

  for (auto& node : _nodes) {
    if (node == _selected) {
      drawList->AddRectFilled(
        node->position() * _scale + _offset + ImGui::GetWindowPos(),
        (node->position() + node->size()) * _scale + _offset + ImGui::GetWindowPos(),
        whiteColor, 2.f);
      drawList->AddText(node->position() * _scale + _offset + ImGui::GetWindowPos() + ImVec2(20, 8),
        blackColor, node->name().c_str());
    }
    else {
      drawList->AddRectFilled(
        node->position() * _scale + _offset + ImGui::GetWindowPos(),
        (node->position() + node->size()) * _scale + _offset + ImGui::GetWindowPos(),
        ImColor(node->color()), 2.f);
      drawList->AddText(node->position() * _scale + _offset + ImGui::GetWindowPos() + ImVec2(20, 8), 
        whiteColor, node->name().c_str());
    }
  }
  */
  ImGui::End();
  //if (_selected)_selected->draw();
}