#include "common.h"
#include "graph.h"
#include "factory.h"

static int CNT = 0;
const int TxGraph::Flags =
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBackground |
ImGuiWindowFlags_NoTitleBar;/* |
  ImGuiWindowFlags_NoInputs;*/

TxGraph::TxGraph(const std::string& name)
  : _name(name)
  , _current(NULL)
  , _connexion(NULL)
  , _active(true)
  , _pick(NULL)
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
    node->setSelected(true);
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

TxConnexion* TxGraph::startConnexion(TxParameter* param, int channel)
{
  if(!_connexion)_connexion = new TxConnexion({ param, NULL, channel, 0 });
  return _connexion;
}

void TxGraph::updateConnexion(TxParameter* param, int channel, bool status)
{
  if (!_connexion) return;
  if (status) {
    _connexion->target = param;
    _connexion->targetChannel = channel;
  }
  else if (param == _connexion->target) {
    _connexion->target = NULL;
    _connexion->targetChannel = 0;
  }
}

void TxGraph::terminateConnexion()
{
  if (!_connexion) return;

  if (_connexion->target) {
    _connexions.push_back(_connexion);
  }
  else {
    delete _connexion;
  }
  _connexion = NULL;
}

TxConnexion* TxGraph::connexion()
{
  return _connexion;
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

TxNode* TxGraph::pick(const ImVec2& pos)
{
  int nodeIdx = _nodes.size() - 1;
  _hovered = NULL;
  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    if (inside(pos, node->position() * _scale + _offset,
      (node->position() + node->size()) * _scale + _offset))
    {
      _hovered = node;
      break;
    }
  }
  
  return _hovered;
}

void TxGraph::select(const ImVec2& pos)
{
  int nodeIdx = _nodes.size() - 1;
  ImGuiIO& io = ImGui::GetIO();
  if (!io.KeyMods)for (auto& node : _nodes)node->setSelected(false);

  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = _nodes[nodeIdx];
    if (inside(pos, node->position() * _scale + _offset,
      (node->position() + node->size()) * _scale + _offset))
    {
      if (io.KeyCtrl) {
        _nodes[nodeIdx]->setSelected(1 - _nodes[nodeIdx]->selected());
      } else {
        _nodes[nodeIdx]->setSelected(true);
      }
      break;
    }
  }
}

void TxGraph::initSpliter()
{
  _splitter.Split(ImGui::GetWindowDrawList(), TxGraph::CNT);
}

void TxGraph::setSplitterChannel(int channel)
{
  _splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);
}

void TxGraph::terminateSplitter()
{
  _splitter.Merge(ImGui::GetWindowDrawList());
}

void TxGraph::_createNodeByType(int type)
{
  TxNode* node = TxFactory::createNodeByType(this, type);
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
      //ImGui::TextColored({ RANDOM_0_1, RANDOM_0_1, RANDOM_0_1, 1 }, TxNode::NodeName[i]);
      if (ImGui::Selectable(TxNode::NodeName[i])) {
        _createNodeByType(i);
      }
    }

   

    ImGui::EndPopup();
  }
}

void TxGraph::_drawConnexion(TxConnexion* connexion)
{
  bool inverse = connexion->source->type() == TxParameter::SAMPLES && 
    !((TxParameterSamples*)connexion->source)->io();

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 p0, p1, p2, p3;
  if (!inverse) {
    p0 = connexion->source->plug(connexion->sourceChannel);
    if (connexion->target) {
      p3 = connexion->target->plug(connexion->targetChannel);
    } else {
      p3 = ImGui::GetMousePos();
    }
    p1 = p0 + ImVec2(100 * _scale, 0);
    p2 = p3 - ImVec2(100 * _scale, 0);
  }
  else {
    p3 = connexion->source->plug(connexion->sourceChannel);
    if (connexion->target) {
      p0 = connexion->target->plug(connexion->targetChannel);
    }
    else {
      p0 = ImGui::GetMousePos();
    }
    p1 = p0 + ImVec2(100 * _scale, 0);
    p2 = p3 - ImVec2(100 * _scale, 0);
  }
  
  drawList->AddBezierCubic(
    p0, p1, p2, p3,
    TX_CONTOUR_COLOR_DEFAULT,
    8.f * _scale
  );

  drawList->AddBezierCubic(
    p0, p1, p2, p3,
    ImColor(ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram]),
    (8.f - TX_CONTOUR_WIDTH) * _scale
  );

  const ImVec2 plugSize(TX_PLUG_WIDTH * _scale * 0.5, (TX_PLUG_HEIGHT + TX_PLUG_DETAIL) * 0.5 * _scale);
  const ImVec2 plugOffset(-TX_PLUG_WIDTH * 0.75 * _scale, 0);
  if (connexion->source->type() == TxParameter::SAMPLES) {
    drawList->AddRect(p0 - plugSize - plugOffset, p0 + plugSize - plugOffset, TX_CONTOUR_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale, 0, TX_CONTOUR_WIDTH * _scale);
    drawList->AddRectFilled(p0 - plugSize - plugOffset, p0 + plugSize - plugOffset, TX_PLUG_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale);
  }
  else {
    drawList->AddCircle(p0, TX_PLUG_WIDTH * _scale * 0.5, TX_CONTOUR_COLOR_DEFAULT, 32, TX_CONTOUR_WIDTH * _scale);
    drawList->AddCircleFilled(p0, TX_PLUG_WIDTH * _scale * 0.5, TX_PLUG_COLOR_DEFAULT, 32);
  }
  if (connexion->target && connexion->target->type() == TxParameter::SAMPLES) {
    drawList->AddRect(p3 - plugSize + plugOffset, p3 + plugSize + plugOffset, TX_CONTOUR_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale, 0, TX_CONTOUR_WIDTH * _scale);
    drawList->AddRectFilled(p3 - plugSize + plugOffset, p3 + plugSize + plugOffset, TX_PLUG_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale);
  }
  else {
    drawList->AddCircle(p3, TX_PLUG_WIDTH * _scale * 0.5, TX_CONTOUR_COLOR_DEFAULT, 32, TX_CONTOUR_WIDTH * _scale);
    drawList->AddCircleFilled(p3, TX_PLUG_WIDTH * _scale * 0.5, TX_PLUG_COLOR_DEFAULT, 32);
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

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  initSpliter();
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
    _drag = 1 - ImGui::IsAnyItemHovered();
  }
  else if (io.MouseReleased[0]) {
    _drag = false;
    terminateConnexion();
  }
  
  if (_drag) {
    if (io.MouseDown[0] && !ImGui::IsDragDropActive()) {
      for (size_t n = 0; n < _nodes.size(); ++n) {
        if (!_nodes[n]->selected())continue;
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
    ImGui::PushID(node->name().c_str());
    if (node == _pick && io.MouseClicked[1]) {
      std::cout << "RIGHT CLICK : " << _pick << std::endl;
      ImGui::OpenPopup((_pick->name() + "Popup").c_str());
    }
    node->draw(&modified);
    ImGui::PopID();
  }

  setSplitterChannel(TxGraph::WIRE);
  if (_connexion && io.MouseDown[0] && ImGui::IsDragDropActive()) {
    _drawConnexion(_connexion);
  }

  for (auto& connexion : _connexions) {
    _drawConnexion(connexion);
  }

  setSplitterChannel(TxGraph::MIDDLE);
  if (ImGui::IsDragDropActive()) {
    drawList->AddRectFilled(pos, pos + size, ImColor({ 0,0,0,100 }));
  }
 
  terminateSplitter();


  ImGui::End();
  ImGui::PopStyleVar(2);
}