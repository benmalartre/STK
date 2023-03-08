#include "sequencer.h"

void Sequencer::Track::setLength(uint64_t length)
{
  _times.resize(length);
};

void Sequencer::Track::setWaveForm(int index)
{
  _generator.setWaveForm(index);
}

void Sequencer::Track::setTime(uint64_t timeIdx, const Time& value)
{
  if(timeIdx >= _times.size()){
    _times.resize(timeIdx+1);
  }
}

stk::StkFloat Sequencer::Track::tick()
{
  return _generator.tick();
}


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

void Sequencer::setLength(uint64_t length)
{
  _length = length;
  for(auto& track: _tracks) {
    track.setLength(_length);
  }
}

void Sequencer::setBPM(uint32_t bpm)
{
  _bpm = bpm;
}

uint32_t Sequencer::numTracks()
{
  return _tracks.size();
}

Sequencer::Track* Sequencer::addTrack()
{
  _tracks.push_back(Track(_length));
  return &_tracks.back();
}


void Sequencer::removeTrack(uint32_t trackIdx)
{
  if(_tracks.size() > trackIdx) {
    _tracks.erase(_tracks.begin()+trackIdx);
  }
}

Sequencer::Track* Sequencer::getTrack(uint32_t trackIdx)
{
  if(_tracks.size() <= trackIdx) {
    std::cerr << "invalid track index !" << std::endl;
    return NULL;
  }
  return &_tracks[trackIdx];
}

void Sequencer::setTime(uint32_t trackIdx, uint64_t timeIdx, const Time& time)
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
  _tracks[trackIdx].setTime(timeIdx, time);
}

void Sequencer::start()
{
  _running = true;
  _time = 0;
}

void Sequencer::stop()
{
  _running = false;
}

uint64_t Sequencer::timeToIndex(double time)
{
  uint64_t index = time * (60 / _bpm * _n);
  return index % _length;
}

stk::StkFloat Sequencer::tick(uint32_t trackIdx, uint64_t timeIdx)
{
  if(!_running) return 0;
  if(trackIdx > _tracks.size()) {
    std::cerr << "Invalid Track Index : " << trackIdx << 
    " (max = " << (_tracks.size() - 1) << ")" << std::endl;
    return 0;
  }
  return _tracks[trackIdx].tick();
}