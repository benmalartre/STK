#include <cmath>
#include "node.h"
#include "sequencer.h"
#include "graph.h"
#include "factory.h"

size_t TxSequencer::TRACK_INDEX = 0;

void updateBPM(TxSequencer* sequencer)
{
  TxTime& time = TxTime::instance();
  time.setBPM(sequencer->bpm());
}

TxSequencer::TxSequencer(uint32_t numTracks, uint32_t bpm, uint64_t length)
  : TxNode(NULL, TxNode::SEQUENCER, "TxSequencer", TX_NUM_CHANNELS)
  , _bpm(bpm)
  , _length(length)
  , _volume(1.f)
  , _running(false)
{
  TxTime& time = TxTime::instance();
  time.setBPM(_bpm);
  setLength(length);
  _params.push_back(new TxParameterBool(this, "Running", &_running, RUNNING));
  _params.push_back(new TxParameterInt(this, "Bpm", 1, 440, &_bpm, BPM, TxParameter::KNOB));
  _params.back()->setCallback(new TxCallback((CALLBACK_FN)&updateBPM, this));
  _params.push_back(new TxParameterFloat(this, "Volume", 0.f, 2.f, &_volume, VOLUME));
  _params.push_back(new TxParameterFloat(this, "Time", -10000.f, 10000.f, &_time, TIME, TxParameter::FLOAT));

  _size = ImVec2(200, 200);

  _tracks.resize(numTracks);
  for (size_t i = 0; i < numTracks; ++i) {
    _tracks[i] = new TxTrack("Track"+std::to_string(TxSequencer::TRACK_INDEX++));
    _tracks[i]->basicGraph();
  }
}

TxSequencer::TxSequencer(uint32_t numTracks)
  : TxSequencer(numTracks, 60, 8)
{
}

TxSequencer::~TxSequencer()
{
}

void TxSequencer::toggleRunning()
{
  _running = !_running;
}

void TxSequencer::setLength(uint64_t length)
{
  _length = length;
  for (size_t i = 0; i < _tracks.size(); ++i) {
    _tracks[i]->setLength(_length);
  }
}

void TxSequencer::setBPM(uint32_t bpm)
{
  _bpm = bpm;
  TxTime& time = TxTime::instance();
  time.setBPM(_bpm);
}

void TxSequencer::setBeat(uint32_t trackIdx, uint64_t beatIdx, const Beat& beat)
{
  if(_length <= beatIdx) {
    std::cerr << "Invalid beat index : " << beatIdx << 
    " (max = " << (_length - 1) << ")" << std::endl;
    return;
  }
  _tracks[trackIdx]->setBeat(beatIdx, beat);
}

void TxSequencer::start()
{
  _running = true;
}

void TxSequencer::stop()
{
  _running = false;
}

const ImVec2& TxSequencer::size()
{
  return _size;
}

Beat* TxSequencer::beat(uint32_t trackIdx, size_t beatIdx)
{
  return _tracks[trackIdx]->beat(beatIdx);
}

TxTrack* TxSequencer::track(size_t index)
{
  if (index < _tracks.size())return _tracks[index];
  else return NULL;
}

void TxSequencer::addTrack()
{
  TxTrack* track = new TxTrack("Track"+std::to_string(TxSequencer::TRACK_INDEX++));
  track->setLength(_length);
  track->basicGraph();
  _tracks.push_back(track);
}

void TxSequencer::removeTrack(TxTrack* track)
{
  if(_current == track)_current = nullptr;
  for(size_t i = 0; i < _tracks.size(); ++i) {
    if(track != _tracks[i]) continue;
    _tracks.erase(_tracks.begin() + i);
    delete track;
    break;
  }
}

stk::StkFloat TxSequencer::tick(unsigned int channel)
{  
  double sample = 0.f;
  
  for (size_t i = 0; i < _tracks.size(); ++i) {
    if (_tracks[i]->active() && _tracks[i]->graph()) {
      _tracks[i]->dirtyAllNodes();
      const Index index = TxTime::instance().index(_tracks[i]->length());
      sample += _tracks[i]->graph()->tick() * _tracks[i]->volume();
    }
  }

  sample *= _volume;
  if(_recorder.recording())
    _recorder.tick(sample);

  _buffer.write(sample);
  
  return sample;
}


void TxSequencer::draw(TxEditor* editor)
{  
  TxTime& time = TxTime::instance();

  if (ImGuiKnobs::KnobInt("Bpm", &_bpm, 1, 220, 1, "%ibpm",
    ImGuiKnobVariant_WiperDot, 0.f, ImGuiKnobFlags_DragHorizontal))
      updateBPM(this);
  
  ImGui::SameLine();

  ImGuiKnobs::Knob("Vol", &_volume, 0.f, 2.f, 0.f, 0,
    ImGuiKnobVariant_WiperDot, 0.f, ImGuiKnobFlags_DragHorizontal);

  ImGui::SameLine();

  ImGui::BeginGroup();
  
  if (ImGui::Button("Reset Time")) time.reset();
  ImGui::SetNextItemWidth(100);
  ImGui::Text("Time : %fs", time.get());

  ImGui::Text("Rate : %fs", time.rate());
  ImGui::Text("Index : %i", time.index(_length).first);


  ImGui::EndGroup();

  ImGui::SameLine();

  ImGui::BeginGroup();
  _buffer.draw();

  ImGui::Checkbox("Running", &_running);
  ImGui::SameLine();
  _recorder.draw();
  ImGui::EndGroup();

}

void TxSequencer::reset()
{
  
}