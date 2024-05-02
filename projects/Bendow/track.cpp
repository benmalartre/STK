#include "common.h"
#include "track.h"
#include "sequencer.h"
#include "graph.h"

TxTrack::TxTrack()
  : TxNode(NULL, TxNode::TRACK, "track")
  , _graph(NULL)
{
  _params.push_back(new TxParameterFloat(this, "Volume", 20.f, 3000.f, &_volume, TxParameter::KNOB));
  _params.push_back(new TxParameterBool(this, "Trigger", &_trigger));
  _params.push_back(new TxParameterFloat(this, "Value", 0.f, 2.f, &_value));
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

stk::StkFrames& TxTrack::tick(stk::StkFrames& frames, unsigned int channel)
{
  const Index index = TxTime::instance().index(_sequence.size());
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

void TxTrack::basicGraph()
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