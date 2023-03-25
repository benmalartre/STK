#include "graph.h"
#include "oscillator.h"

static int CNT = 0;
const int TxGraph::Flags =
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoNav |
//ImGuiWindowFlags_NoBackground |
ImGuiWindowFlags_NoTitleBar;/* |
  ImGuiWindowFlags_NoInputs;*/

TxGraph::TxGraph(const std::string& name)
  : _name(name)
  , _current(NULL)
  , _active(true)
  , _pick(NONE)
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
  return point[0] >= bmin[0] && point[0] <= bmax[0] &&
    point[1] >= bmin[1] && point[1] <= bmax[1];
}

int TxGraph::pick(const ImVec2& pos)
{
  int nodeIdx = _nodes.size() - 1;

  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    if (inside(pos, node->position() * _scale + _offset,
      (node->position() + node->size()) * _scale + _offset))
    {
      _hovered = nodeIdx;
      if ((pos.y - (node->position().y * _scale + _offset.y)) < TX_TITLE_Y * 2) {
        return TxGraph::HEAD;
      }
      return TxGraph::BODY;
    }
  }
  _hovered = -1;
  return TxGraph::NONE;
}

void TxGraph::select(const ImVec2& pos)
{
  int nodeIdx = _nodes.size() - 1;
  ImGuiIO& io = ImGui::GetIO();
  if (!io.KeyMods)for (auto& selected : _selection)selected = false;

  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    if (inside(pos, node->position() * _scale + _offset,
      (node->position() + node->size()) * _scale + _offset))
    {
      if (io.KeyCtrl) {
        _selection[nodeIdx] = 1 - _selection[nodeIdx];
      } else {
        _selection[nodeIdx] = true;
      }
      break;
    }
  }
}

void TxGraph::_drawGrid()
{
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
  float GRID_SZ = 64.0f * _scale;
  ImVec2 win_pos = ImGui::GetCursorScreenPos();
  ImVec2 canvas_sz = ImGui::GetWindowSize();
  for (float x = fmodf(_offset.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
    drawList->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
  for (float y = fmodf(_offset.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
    drawList->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
}

void TxGraph::_drawPopup()
{
  static const char* name = "GraphNodesMenu";
  if (ImGui::Button("Show popup")) {
    ImGui::OpenPopup(name);
  }

  if (ImGui::BeginPopup(name)) {
    for (size_t i = 0; i < TxNode::NumNode; ++i) {
      ImGui::TextColored({ RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1 }, TxNode::NodeName[i]);
    }

    if (ImGui::Selectable("Oscillator")) {
      new TxOscillator(this, (std::string("zob")+std::to_string(CNT++)).c_str());
    }

    ImGui::EndPopup();
  }
}

void TxGraph::draw()
{
  static float h = 0.f;
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing * _scale);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, style.ItemInnerSpacing * _scale);
  const ImVec2 pos(0, h);
  const ImVec2 size = io.DisplaySize - pos;
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(size);
  ImGui::Begin(_name.c_str(), NULL, TxGraph::Flags);

  if (io.MouseWheel) {
    _scale = ImClamp(_scale + io.MouseWheel * 0.1f, 0.1f, 4.f);
  }
  io.FontGlobalScale = _scale;
  _drawGrid();
 
  if (!io.MouseDown[0]) {
    _pick = pick(ImGui::GetMousePos());
  }
  
  if (io.MouseClicked[0]) {
    select(ImGui::GetMousePos());
    _drag = _pick == TxGraph::HEAD;
  } else if (io.MouseReleased[0]) {
    _drag = false;
  }

  if (_pick == TxGraph::HEAD || _drag) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
  }
  else {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
  }

  if (_drag) {
    if (io.MouseDown[0] && !ImGui::IsDragDropActive()) {
      for (size_t n = 0; n < _nodes.size(); ++n) {
        if (!_selection[n])continue;
        _nodes[n]->setPosition(_nodes[n]->position() + io.MouseDelta * 1.f / _scale);
      }
    }
  }
  if (io.MouseDown[2]) {
    _offset += io.MouseDelta;
  }
  _drawPopup();
  
  bool modified = false;
  for (size_t n = 0; n < _nodes.size(); ++n) {
    TxNode* node = _nodes[n];
    node->draw(_selection[n], &modified);
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
 
  ImGui::End();
  ImGui::PopStyleVar(2);
}