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

const ImVec2& TxGraph::offset()
{
  return _offset;
}

const float& TxGraph::scale()
{
  return _scale;
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
    _selection.push_back(true);
  }
}

void TxGraph::removeNode(TxNode* node) 
{
  int idx = index(node);
  if(idx >= 0) {
    _nodes.erase(_nodes.begin() + idx);
    _selection.erase(_selection.begin() + idx);
    delete node;
  }
}

void TxGraph::addConnexion(TxConnexion* connexion)
{
  _connexions.push_back(connexion);
}

void TxGraph::removeConnexion(TxConnexion* connexion)
{
  for (size_t c = 0; c < _connexions.size(); ++c) {
    if (_connexions[c] == connexion) {
      _connexions.erase(_connexions.begin() + c);
      delete connexion;
      return;
    }
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

  int nodeIdx = _nodes.size()-1;
  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    if(inside(pos, node->position() * _scale + _offset, 
      (node->position() + node->size()) * _scale + _offset))
      {
        _selected = node;
        if (_selected && _selected != _nodes.back()) {
          std::swap(_nodes[index(_selected)], _nodes.back());
        }
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
  static float h = 0.f;
  ImGuiIO& io = ImGui::GetIO();
  const ImVec2 pos(0, h);
  const ImVec2 size = io.DisplaySize - pos;
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(size);
  ImGui::Begin(_name.c_str(), NULL, TxGraph::Flags);
  io.FontGlobalScale = _scale;

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
  float GRID_SZ = 64.0f * _scale;
  ImVec2 win_pos = ImGui::GetCursorScreenPos();
  ImVec2 canvas_sz = ImGui::GetWindowSize();
  for (float x = fmodf(_offset.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
    drawList->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
  for (float y = fmodf(_offset.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
    drawList->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
 
  int picked = TxGraph::NONE;
  ImVec2 offset;
  if(io.MouseWheel) {
    _scale = ImClamp(_scale + io.MouseWheel * 0.1f, 0.1f, 4.f);
  }

  if (io.MouseClicked[0]) {
    picked = pick(ImGui::GetMousePos());
    if (_selected) {
      offset = ImGui::GetMousePos() - _selected->position();
    }
  }

  if (!picked) {
    if (io.MouseDown[0] && _selected) {
      _selected->setPosition(ImGui::GetMousePos() - offset);
    }
    else if (io.MouseDown[2]) {
      _offset += io.MouseDelta;
    }
  }

  bool modified = false;
  for (auto& node : _nodes) {
    node->draw(RANDOM_0_1 > 0.5, &modified);
  }

  ImDrawList* foregroundList = ImGui::GetForegroundDrawList();
  if (io.MouseDown[0] && ImGui::IsDragDropActive()) {
    foregroundList->AddLine(io.MouseClickedPos[0], ImGui::GetMousePos(), ImColor({ 255,0,0,255 }), 8.f * _scale);
  }

  for (auto& connexion : _connexions) {
    foregroundList->AddLine(
      connexion->source->plug(), 
      connexion->target->plug(), 
      ImColor({ RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1.f }), 
      8.f * _scale);
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