#include "common.h"
#include "graph.h"
#include "factory.h"
#include "editor.h"
#include "sequencer.h"


static int CNT = 0;
const int TxEditor::Flags =
ImGuiWindowFlags_NoResize |
ImGuiWindowFlags_NoCollapse |
ImGuiWindowFlags_NoMove |
ImGuiWindowFlags_NoNav |
ImGuiWindowFlags_NoBackground |
ImGuiWindowFlags_NoTitleBar;
/*|ImGuiWindowFlags_NoInputs;*/

TxEditor::TxEditor(TxGraph* graph, bool navigatable)
  : _current(NULL)
  , _connexion(NULL)
  , _pick(NULL)
  , _drag(false)
  , _pan(false)
  , _graph(graph)
  , _offset(ImVec2(0,0))
  , _scale(1.f)
  , _navigatable(navigatable)

{};

TxNode* TxEditor::current()
{
  return _current;
}

const ImVec2& TxEditor::pos()
{
  return ImGui::GetWindowPos();
}

const ImVec2& TxEditor::offset()
{
  return _offset;
}

const float& TxEditor::scale()
{
  return _scale;
}

TxConnexion* TxEditor::startConnexion(TxParameter* param, int channel)
{
  if(!_connexion)_connexion = new TxConnexion({ param, NULL, channel, 0 });
  return _connexion;
}

void TxEditor::updateConnexion(TxParameter* param, int channel, bool status)
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

void TxEditor::terminateConnexion()
{
  if (!_connexion) return;

  if (_connexion->target) {
    _graph->addConnexion(_connexion);
  }
  else {
    delete _connexion;
  }
  _connexion = NULL;
}

TxConnexion* TxEditor::connexion()
{
  return _connexion;
}

static bool inside(const ImVec2& point, const ImVec2& bmin, const ImVec2& bmax)
{
  return point[0] >= bmin[0] && point[0] <= bmax[0] &&
    point[1] >= bmin[1] && point[1] <= bmax[1];
}

TxNode* TxEditor::pick(const ImVec2& p)
{
  std::vector<TxNode*>& nodes = _graph->nodes();
  int nodeIdx = nodes.size() - 1;
  _hovered = NULL;
  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = nodes[nodeIdx];
    if (inside(p, node->position(), node->position() + node->size()))
    {
      _hovered = node;
      break;
    }
  }
  
  return _hovered;
}

void TxEditor::select(const ImVec2& p)
{
  std::vector<TxNode*>& nodes = _graph->nodes();
  int nodeIdx = nodes.size() - 1;
  ImGuiIO& io = ImGui::GetIO();
  if (!io.KeyMods)for (auto& node : nodes)node->setSelected(false);

  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = nodes[nodeIdx];
    if (inside(p, node->position(), node->position() + node->size()))
    {
      if (io.KeyCtrl) {
        nodes[nodeIdx]->setSelected(1 - nodes[nodeIdx]->selected());
      } else {
        nodes[nodeIdx]->setSelected(true);
      }
      break;
    }
  }
}

void TxEditor::initSpliter()
{
  _splitter.Split(ImGui::GetWindowDrawList(), TxEditor::CNT);
}

void TxEditor::setSplitterChannel(int channel)
{
  _splitter.SetCurrentChannel(ImGui::GetWindowDrawList(), channel);
}

void TxEditor::terminateSplitter()
{
  _splitter.Merge(ImGui::GetWindowDrawList());
}

void TxEditor::handleInput()
{
  ImGuiIO& io = ImGui::GetIO();

  if (io.MouseWheel && _navigatable) {
    _scale = ImClamp(_scale + io.MouseWheel * 0.1f, 0.1f, 4.f);
  }
  if (!io.MouseDown[0]) {
    _pick = pick(ImGui::GetMousePos() * _scale - _offset);
  }

  if (io.MouseClicked[0]) {
    select(ImGui::GetMousePos() * _scale - _offset);
    _drag = 1 - ImGui::IsAnyItemHovered();
    if(io.KeySuper) _pan = true;
  } else if (io.MouseReleased[0]) {
    _drag = false;
    _pan = false;
    terminateConnexion();
  }
  if (_pan) _offset += io.MouseDelta;

  if (!_drag)return;

  if (io.MouseDown[0] && !ImGui::IsDragDropActive()) {
    std::vector<TxNode*>& nodes = _graph->nodes();
    for (size_t n = 0; n < nodes.size(); ++n) {
      if (!nodes[n]->selected())continue;
      nodes[n]->setPosition(nodes[n]->position() + io.MouseDelta * 1.f / _scale);
    }
  }
}

void TxEditor::setCurrent(TxNode* node)
{
  _current = node;
}

void TxEditor::setGraph(TxGraph* graph)
{
  _graph = graph;
}

TxGraphEditor::TxGraphEditor(TxGraph* graph)
  : TxEditor(graph, true)
{};

TxGraphEditor::~TxGraphEditor()
{
};

bool TxGraphEditor::contains(const TxNode* node)
{
  if(_graph)
    for(auto& n: _graph->nodes())
      if(node == n)return true;
  
  return false;
}

void TxGraphEditor::_createNodeByType(int type)
{
  TxNode* node = TxFactory::createNodeByType(_graph, type);
}

void TxGraphEditor::_drawFrame()
{
  ImVec2 s = ImGui::GetWindowSize();
  ImVec2 p = ImGui::GetWindowPos();
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImU32 CANVAS_COLOR = IM_COL32(200,200,200, 40);
  drawList->AddRectFilled(ImVec2(p.x+32, p.y+24), 
    ImVec2(p.x+s.x-32, p.y+s.y-10), CANVAS_COLOR, 4.f);

}


void TxGraphEditor::_drawGrid()
{
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 p = ImGui::GetWindowPos() + _offset;
  size_t scl = _scale * 100;
  size_t px = static_cast<int>(p.x) % scl;
  size_t py = static_cast<int>(p.y) % scl;

  p = ImVec2(px, py);

  ImVec2 s = ImGui::GetWindowSize();
  ImU32 GRID_COLOR = IM_COL32(120, 120, 120, 60);
  float GRID_SZ = 100.f * _scale;
  ImVec2 c(s.x / GRID_SZ, s.y / GRID_SZ);
  
  for (float x = 0; x < s.x; x += GRID_SZ)
    drawList->AddLine(ImVec2(x, 0.0f) + p, ImVec2(x, s.y) + p, GRID_COLOR);
  for (float y = 0; y < s.y; y += GRID_SZ)
    drawList->AddLine(ImVec2(0.0f, y) + p, ImVec2(s.x, y) + p, GRID_COLOR);

}

void TxGraphEditor::_drawPopup()
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

void TxGraphEditor::_drawNode(TxNode* node, bool* modified)
{
  ImGuiIO& io = ImGui::GetIO();
  ImGui::PushID(node->name().c_str());
  if (node == _pick && io.MouseClicked[1]) {
    ImGui::OpenPopup((_pick->name() + "Popup").c_str());
  }
  node->draw(this, modified);
  ImGui::PopID();
}

void TxGraphEditor::_drawConnexion(TxConnexion* connexion)
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
    TX_CONNEXION_WIDTH * _scale
  );

  const ImVec2 plugSize(TX_PLUG_WIDTH * _scale * 0.5, (TX_PLUG_HEIGHT + TX_PLUG_DETAIL) * 0.5 * _scale);
  const ImVec2 plugOffset(-TX_PLUG_WIDTH * 0.75 * _scale, 0);
  if (connexion->source->type() == TxParameter::SAMPLES) {
    drawList->AddRectFilled(p0 - plugSize - plugOffset, p0 + plugSize - plugOffset, TX_PLUG_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale);
  }
  else {
    drawList->AddCircleFilled(p0, TX_PLUG_WIDTH * _scale * 0.5, TX_PLUG_COLOR_DEFAULT, 32);
  }
  if (connexion->target && connexion->target->type() == TxParameter::SAMPLES) {
    drawList->AddRect(p3 - plugSize + plugOffset, p3 + plugSize + plugOffset, TX_CONTOUR_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale, 0, TX_CONTOUR_WIDTH * _scale);
    drawList->AddRectFilled(p3 - plugSize + plugOffset, p3 + plugSize + plugOffset, TX_PLUG_COLOR_DEFAULT, TX_NODE_ROUNDING * _scale);
  }
  else {
    drawList->AddCircleFilled(p3, TX_PLUG_WIDTH * _scale * 0.5, TX_PLUG_COLOR_DEFAULT, 32);
  }
}

void TxGraphEditor::draw()
{

  std::vector<TxNode*>& nodes = _graph->nodes();
  std::vector<TxConnexion*>& connexions = _graph->connexions();
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing * _scale);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, style.ItemInnerSpacing * _scale);
  _pos =  ImVec2(0, SplitterHeight);
  _size = io.DisplaySize - _pos;
  ImGui::SetNextWindowPos(_pos);
  ImGui::SetNextWindowSize(_size);
  ImGui::Begin("editor", NULL, TxEditor::Flags);

  ImGui::SetWindowFontScale(_scale);

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  initSpliter();
  if(inside(io.MousePos, _pos, _pos + _size))handleInput();

  _drawFrame();
  _drawGrid();
  _drawPopup();
  
  bool modified = false;

  for (size_t n = 0; n < nodes.size(); ++n) {
    _drawNode(nodes[n], &modified);
  }

  setSplitterChannel(TxEditor::FOREGROUND);
  if (_connexion && io.MouseDown[0] && ImGui::IsDragDropActive()) {
    _drawConnexion(_connexion);
  }

  for (auto& connexion : connexions) {
    _drawConnexion(connexion);
  }

  setSplitterChannel(TxEditor::BACKGROUND);
  if (ImGui::IsDragDropActive()) {
    drawList->AddRectFilled(_pos, _pos + _size, IM_COL32(0,0,0,100));
  }
 
  terminateSplitter();


  ImGui::End();
  ImGui::PopStyleVar(2);

}

 TxSequencerEditor::TxSequencerEditor(TxSequencer* sequencer)
  : TxEditor(NULL),
  _sequencer(sequencer)
 {
 }

 TxSequencerEditor::~TxSequencerEditor()
{
};

 void TxSequencerEditor::draw()
{
  ImGuiIO& io = ImGui::GetIO();
  if(_sequencer)_sequencer->draw();
}