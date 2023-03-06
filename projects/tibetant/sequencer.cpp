#include "sequencer.h"

Sequencer::Sequencer()
  : _bpm(60)
  , _n(4)
  , _length(8)
  , _time(0.0)
  , _running(false)
{
}

Sequencer::Sequencer(uint32_t bpm, uint32_t n, uint64_t length)
  : _bpm(bpm)
  , _n(n)
  , _length(length)
  , _time(0.0)
  , _running(false)
{
}

void Sequencer::SetLength(uint64_t length)
{
  _length = length;
  for(auto& track: _tracks) {
    track.resize(_length);
  }
}

void Sequencer::SetBPM(uint32_t bpm)
{
  _bpm = bpm;
}

uint32_t Sequencer::NumTracks()
{
  return _tracks.size();
}

Sequencer::Track* Sequencer::AddTrack()
{
  _tracks.push_back(Track(_length));
  return &_tracks.back();
}


void Sequencer::RemoveTrack(uint32_t trackIdx)
{
  if(_tracks.size() > trackIdx) {
    _tracks.erase(_tracks.begin()+trackIdx);
  }
}

void Sequencer::SetTime(uint32_t trackIdx, uint64_t timeIdx, const Time& time)
{
  if(_tracks.size() <= trackIdx) {
    std::cerr << "Invalid Track Index : " << trackIdx << 
    " (max = " << (_tracks.size() - 1) << ")" << std::endl;
    return;
  }
  if(_length <= timeIdx) {
    std::cerr << "Invalid Time Index : " << timeIdx << 
    " (max = " << (_length - 1) << ")" << std::endl;
    return;
  }
  _tracks[trackIdx][timeIdx] = time;
}

void Sequencer::Start()
{
  _running = true;
  _time = 0;
}

void Sequencer::Stop()
{
  _running = false;
}

void Sequencer::Tick()
{
  _time++;
}

uint64_t Sequencer::TimeToIndex(double time)
{
  uint64_t index = time * (60 / _bpm * _n);
  return index % _length;
}

int Sequencer::Get(uint32_t trackIdx, uint64_t timeIdx)
{
  if(!_running) return 0;
  if(trackIdx > _tracks.size()) {
    std::cerr << "Invalid Track Index : " << trackIdx << 
    " (max = " << (_tracks.size() - 1) << ")" << std::endl;
    return 0;
  }
  return (int)(_tracks[trackIdx][timeIdx].to_ulong());
}