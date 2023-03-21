#include "graph.h"

TxGraph::TxGraph(const std::string& name) 
  : _name(name)
  , _current(NULL)
  , _active(true)
  , _offset(ImVec2(100,100))
  , _scale(ImVec2(1,1))
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

TxNode* TxGraph::getCurrent()
{
  return _current;
}

std::vector<TxNode*>& TxGraph::getNodes()
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
  return point[0] >= bmin[0] && point[0] <= bmax[0] &&
    point[1] >= bmin[1] && point[1] <= bmax[1];
}


void TxGraph::pick(const ImVec2& pos)
{
  std::cout << "pick position : " << pos[0] << "," << pos[1] << std::endl;
  for(auto& node: _nodes) {
    if(inside(pos, node->position() * _scale + _offset, 
      (node->position() + node->size()) * _scale + _offset))
      {
        _current = node;
        break;
      }
  }
}


void TxGraph::draw()
{
  static ImU32 whiteColor = ImColor({ 255,255,255,255 });
  static ImU32 blackColor = ImColor({ 0,0,0,255 });
  ImGui::Begin(_name.c_str(), NULL);
  ImDrawList* drawList = ImGui::GetWindowDrawList();

  ImGuiIO& io = ImGui::GetIO();
  if (io.MouseClicked[0] && inside(ImGui::GetMousePos(), ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(1024,1024))) {
    pick(io.MouseClickedPos[0] - ImGui::GetWindowPos());
  }

  for (auto& node : _nodes) {
    if (node == _current) {
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
  ImGui::End();
}