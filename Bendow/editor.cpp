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
  ImGuiWindowFlags_NoTitleBar;


TxEditor::TxEditor(TxGraph* graph, bool navigatable)
  : _current(NULL)
  , _connexion(NULL)
  , _pick(NULL)
  , _drag(false)
  , _pan(false)
  , _graph(graph)
  , _offset(ImVec2(0,0))
  , _scale(1.f)
  , _invScale(1.f)
  , _navigatable(navigatable)
{};

TxNode* TxEditor::current()
{
  return _current;
}

const ImVec2& TxEditor::pos()
{
  return _pos;
}

const ImVec2& TxEditor::offset()
{
  return _offset;
}

const float& TxEditor::scale()
{
  return _scale;
}

void TxEditor::startConnexion(TxParameter* param, int channel)
{
  _connexion = new TxConnexion({ param, NULL, channel });
}

void TxEditor::updateConnexion(TxParameter* param, int channel)
{
  if (!_connexion) return;

  _connexion->target = param;
}

void TxEditor::terminateConnexion()
{
  if (!_connexion) return;

  if (_connexion->target) {
    _connexion->source->node()->connect(_connexion->target->node(), 
    _connexion->target->name(), _connexion->channel);

    if(_connexion->target->node()->type() == TxNode::GRAPH) {
      ((TxGraph*)_connexion->target->node())->setCurrent(_connexion->source->node());
    }
  }

  delete _connexion;
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
  if (!io.KeyMods)for (auto& node : nodes) {
    node->setSelected(false);
    _selected.clear();
  }

  for (; nodeIdx >= 0; --nodeIdx) {
    TxNode* node = nodes[nodeIdx];
    if (inside(p, node->position(), node->position() + node->size()))
    {
      if (io.KeyCtrl) {
        const bool selected = nodes[nodeIdx]->selected();
        if(selected) {
          nodes[nodeIdx]->setSelected(false);
          _selected.erase(std::find(nodes.begin(), nodes.end(), nodes[nodeIdx]));
        } else {
          nodes[nodeIdx]->setSelected(true);
          _selected.push_back(nodes[nodeIdx]);
        }
      } else {
        nodes[nodeIdx]->setSelected(true);
        _selected.push_back(nodes[nodeIdx]);
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
    _invScale = 1.f / _scale;
  }
  if (!io.MouseDown[0]) {
    _pick = pick((ImGui::GetMousePos() - pos() - _offset) * _invScale);
  }

  if (io.MouseClicked[0]) {
    if(io.KeyShift) _pan = true;
    else _drag = 1 - ImGui::IsAnyItemHovered();
    select((ImGui::GetMousePos() - pos() - _offset) * _invScale);
  } else if (io.MouseReleased[0]) {
    _drag = false;
    _pan = false;
    terminateConnexion();
  }

  if (!_drag && !_pan)return;

  if (_pan) _offset += io.MouseDelta;

  if (io.MouseDown[0] && !_pan && !ImGui::IsDragDropActive()) {
    std::vector<TxNode*>& nodes = _graph->nodes();
    for (size_t n = 0; n < nodes.size(); ++n) {
      if (!nodes[n]->selected())continue;
      nodes[n]->setPosition(nodes[n]->position() + io.MouseDelta * 1.f / _scale);
    }
  } 
}

void TxEditor::setCurrent(TxTrack* track)
{
  _current = track;
  if(_current)setGraph(_current->graph());
  else setGraph(nullptr);
}

void TxEditor::setGraph(TxGraph* graph)
{
  _graph = graph;
}

TxTrack* TxEditor::getCurrent()
{
  return _current;
}

TxGraph* TxEditor::getGraph()
{
  return _graph;
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
      if(node == n) return true;
  
  return false;
}

void TxGraphEditor::deleteSelectedNodes()
{
  if(_graph && _selected.size())
    for(TxNode* node: _selected)
      _graph->removeNode(node);
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
  ImU32 color = IM_COL32(100,100,100, 255);
  //drawList->AddRectFilled(ImVec2(p.x+32, p.y+24), 
  //  ImVec2(p.x+s.x-32, p.y+s.y-10), color, 4.f);

  ImRect outer(p, p + s);
  ImRect inner(ImVec2(p.x+32, p.y+24), ImVec2(p.x+s.x-32, p.y+s.y-10));

  ImGui::RenderRectFilledWithHole(drawList, outer, inner, color, 0.f);

  ImVec2 pMin, pMax;

  if(_current) {
    TxParameterSamples* inputs = (TxParameterSamples*)_current->parameter("Output");
    pMin = _pos + ImVec2(24, 100);
    pMax = pMin + ImVec2(8, 10);

    inputs->draw(this, pMin, pMax, 1.f, 0);

    pMin += ImVec2(0, 32);
    pMax += ImVec2(0, 32);
    inputs->draw(this, pMin, pMax, 1.f, 1);


    TxParameterSamples* output = (TxParameterSamples*)_current->graph()->parameter("Samples");
    pMin = _pos + ImVec2(_size.x - 24, 100);
    pMax = pMin + ImVec2(8, 10);

    output->draw(this, pMin, pMax, 1.f, 0);
  }
}

void TxGraphEditor::_drawGrid()
{
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 p = _offset;
  float scl = _scale * 50;
  int px = static_cast<int>(p.x) % (int)scl;
  int py = static_cast<int>(p.y) % (int)scl;

  p = ImGui::GetWindowPos() + ImVec2(px-scl, py-scl);

  ImVec2 s = ImGui::GetWindowSize();
  ImU32 color = IM_COL32(120, 120, 120, 60);
  
  for (float x = 0; x <= s.x; x += scl)
    drawList->AddLine(ImVec2(x, 0.0f) + p, ImVec2(x, s.y) + p, color);
  for (float y = 0; y <= s.y; y += scl)
    drawList->AddLine(ImVec2(0.0f, y) + p, ImVec2(s.x, y) + p, color);

}

void TxGraphEditor::_drawPopup()
{
  ImGui::SetCursorPos(ImVec2(10, 10));
  static const char* name = "GraphNodesMenu";
  if (ImGui::Button("Nodes")) {
    ImGui::OpenPopup(name);
  }

  if (ImGui::BeginPopup(name)) {
    for (size_t i = 0; i < TxNode::NumExposedNode; ++i) {
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
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 p0, p1, p2, p3;
  p0 = connexion->source->plug(connexion->channel);
  if (connexion->target) {
    p3 = connexion->target->plug(0);
  } else {
    p3 = ImGui::GetMousePos();
  }
  p1 = p0 + ImVec2(100 * _scale, 0);
  p2 = p3 - ImVec2(100 * _scale, 0);
  
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

void TxGraphEditor::resize(size_t splitterHeight)
{
  ImGuiIO& io = ImGui::GetIO();
  _pos = ImVec2(0, splitterHeight);
  _size = ImVec2(io.DisplaySize.x, io.DisplaySize.y - splitterHeight);

}

void TxGraphEditor::draw()
{

  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing * _scale);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, style.ItemInnerSpacing * _scale);
  ImGui::SetNextWindowPos(_pos);
  ImGui::SetNextWindowSize(_size);
  ImGui::Begin("Graph", NULL, TxEditor::Flags);

  ImGui::SetWindowFontScale(1.f);
  _drawGrid();

  ImGui::SetWindowFontScale(_scale);

  ImDrawList* drawList = ImGui::GetWindowDrawList();
  initSpliter();
  if(inside( ImGui::GetMousePos(), _pos, _pos + _size))handleInput();

  bool modified = false;

  if(_graph) {
    std::vector<TxNode*>& nodes = _graph->nodes();
    std::vector<TxConnexion*>& connexions = _graph->connexions();

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

    if(_graph->current())_graph->current()->draw(this, &modified);
  } 
  
  setSplitterChannel(TxEditor::BACKGROUND);
  if (ImGui::IsDragDropActive()) {
    drawList->AddRectFilled(_pos, _pos + _size, IM_COL32(0,0,0,60));
  }
 
  terminateSplitter();

  ImGui::SetWindowFontScale(1.f);
  _drawFrame();
  _drawPopup();


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

void TxSequencerEditor::resize(size_t splitterHeight)
{
  ImGuiIO& io = ImGui::GetIO();
  _pos = ImVec2(0, 0);
  _size = ImVec2(io.DisplaySize.x, splitterHeight);

}

 void TxSequencerEditor::draw()
{
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.ItemSpacing);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, style.ItemInnerSpacing);
  ImGui::SetNextWindowPos(_pos);
  ImGui::SetNextWindowSize(_size);
  ImGui::Begin("Sequencer", NULL, Flags);

  ImGui::SetWindowFontScale(1.f);

  TxTime& time = TxTime::instance();
  float t = time.get();
  
  _sequencer->draw(this);


  TxTrack* removeTrack = nullptr;
  std::vector<TxTrack*>& tracks = _sequencer->tracks();
  Index index = TxTime::instance().index(tracks[0]->length());


  for (size_t i = 0; i < tracks.size(); ++i) {
    //ImGui::PushID(tracks[i]->name().c_str());
    //ImGui::SetCursorPosX(10);
    const bool selected = (tracks[i] == _current);
    ImGui::BeginGroup();

    //ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_Framed;
    bool expended = (ImGui::TreeNode(tracks[i]->name().c_str()));
    ImGui::SameLine();
    if(ImGui::Button(ICON_FA_TRASH))
      removeTrack = tracks[i];
    
    if(expended) 
    {
      _current = tracks[i];

      ImGui::BeginGroup();
      ImVec4* colors = style.Colors;
      const bool active = tracks[i]->active();
      ImGui::PushStyleColor(ImGuiCol_Text, active ? style.Colors[ImGuiCol_TextDisabled] : style.Colors[ImGuiCol_Text]);
      
      if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_ButtonActive]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_ButtonActive]);
      }
      
      if(ImGui::Button(ICON_FA_HEADPHONES, ImVec2(32, 32)))
        tracks[i]->toggleActive();

      if(active) ImGui::PopStyleColor(2);
      ImGui::PopStyleColor();
      TxParameter* volume = tracks[i]->parameter("Volume");
      volume->draw(this);
      ImGui::EndGroup();
      //ImGui::PopID();

      ImGui::SameLine();
      //ImGui::SetCursorPosX(70);
      for (size_t j = 0; j < tracks[i]->length(); ++j) {
        _drawBeat(tracks[i], expended, j, index.second, (j == index.first));
        if (i < tracks[i]->length() - 1)  ImGui::SameLine();
      }

      ImGui::TreePop();
    } else {
      ImGui::SameLine();
      //ImGui::SetCursorPosX(70);
      for (size_t j = 0; j < tracks[i]->length(); ++j) {
        _drawBeat(tracks[i], expended, j, index.second, (j == index.first));
        if (i < tracks[i]->length() - 1)  ImGui::SameLine();
      }
    }
    ImGui::EndGroup();
  }

  if(removeTrack) {
    if(_current == removeTrack)_current = nullptr;
    _sequencer->removeTrack(removeTrack);
    TxGraphEditor* editor = getGraphEditor();
    editor->setCurrent(nullptr);
  }
  

  //ImGui::SetCursorPosX(10);

  if(ImGui::Button(ICON_FA_PLUS, ImVec2(60, 16)))
    _sequencer->addTrack();
  

  ImGui::End();
  ImGui::PopStyleVar(2);
}

bool TxSequencerEditor::_drawBeat(TxTrack* track, bool expended, uint32_t beatIdx, uint32_t bitIdx, bool current)
{
  const size_t beatWidth = (_size.x - 120) / (track->length() + 1);
  const size_t bitWidth = beatWidth / NumBits;
  bool modified = false;
  const std::string hiddenPrefix = "##" + std::to_string(beatIdx);
  
  Beat* beat = track->beat(beatIdx);
  if(expended) {
    ImGui::BeginGroup();

    if (ImGui::VSliderFloat((hiddenPrefix + "Slider").c_str(), ImVec2(beatWidth, 120),
      &beat->second, 0, 255))modified = true;
  }

  ImGui::BeginGroup();
  
  const ImGuiStyle& style = ImGui::GetStyle();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));

  for(size_t i = 0; i < 4; ++i) {
    bool bit = BIT_CHECK(beat->first, i);
    const ImVec4 btnColor = (current && (i == bitIdx))? 
      (bit ? ImVec4(1, 0, 0, 1): ImVec4(1, 0.75, 0, 1)) 
      : (bit ? ImVec4(1, 0.75, 0, 1) : style.Colors[ImGuiCol_FrameBg]);

    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
    if (ImGui::Button((hiddenPrefix + std::to_string((intptr_t)(void*)track) + "Btn" + std::to_string(i)).c_str(), ImVec2(bitWidth, 12))) {
      BIT_FLIP(beat->first, i);
      modified = true;
    }
    ImGui::PopStyleColor();
    if(i < 3)ImGui::SameLine();
  }
  ImGui::PopStyleVar();

  ImGui::EndGroup();

  if(expended)ImGui::EndGroup();
  
  return modified;
}