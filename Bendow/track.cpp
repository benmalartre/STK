#include "common.h"
#include "track.h"
#include "sequencer.h"
#include "graph.h"

TxTrack::TxTrack(const std::string& name)
  : TxNode(NULL, TxNode::TRACK, name, 2)
  , _graph(NULL)
{
  TxParameterFloat* volume = new TxParameterFloat(this, "Volume", 0.f, 2.f, &_volume, TxParameter::KNOB);
  _params.push_back(volume);
  volume->setSize(TX_KNOB_SMALL_SIZE);
  volume->set(1.f);
}

TxTrack::~TxTrack()
{

}

stk::StkFloat TxTrack::tick(unsigned int channel)
{
  Index index = TxTime::instance().index(_sequence.size());

  const Beat& beat = _sequence[index.first];
  if(channel == 0)return BIT_CHECK(beat.first, index.second);
  else if(channel == 1) return beat.second;
}

void TxTrack::_drawImpl(TxEditor* editor, bool* modified)
{
  ImGui::BeginGroup();
  TxParameter* value = _params[TxTrack::VALUE];
  if (value->draw(editor) && modified)*modified = true;
  ImGui::EndGroup();

  ImGui::SameLine();
  TxNode::_drawOutput(editor);
}

void TxTrack::reset()
{
}

void TxTrack::basicGraph()
{
  _graph = new TxGraph(this, "graph");
  _graph->basic(this);
}

void TxTrack::sequencedGraph()
{
  _graph = new TxGraph(this, "graph");
  _graph->sequenced(this);
}

void TxTrack::setLength(size_t length)
{
  _sequence.resize(length);
}

void TxTrack::setBeat(size_t beatIdx, const Beat& beat)
{
    _sequence[beatIdx] = beat;
}