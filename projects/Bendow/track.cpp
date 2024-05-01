#include "track.h"
#include "sequencer.h"
#include "graph.h"

TxTrack::TxTrack()
  : TxNode(NULL, TxNode::TRACK, "track")
  , _graph(NULL)
{
}

TxTrack::~TxTrack()
{

}

stk::StkFloat TxTrack::tick(unsigned int)
{
  return 0.f;
}

stk::StkFrames& TxTrack::tick(stk::StkFrames& frames, unsigned int channel)
{
  for(size_t i = 0; i < frames.size(); ++i) {
    frames[i] = 0.f;
  }
  return frames;
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

void TxTrack::setBasicGraph()
{
  _graph = new TxGraph(this, "graph");
  _graph->basic(this);
}

void TxTrack::setLength(size_t length)
{
  _sequence.resize(length);
}

void TxTrack::setBeat(size_t beatIdx, const Beat& beat)
{
    _sequence[beatIdx] = beat;
}