#include "sequencer.h"

void Sequencer::Track::setLength(uint64_t length)
{
  _times.resize(length);
};

void Sequencer::Track::setWaveForm(int index)
{
  _generator->setWaveForm(index);
}

void Sequencer::Track::setTime(uint64_t timeIdx, const Time& value)
{
  if(timeIdx >= _times.size()){
    _times.resize(timeIdx+1);
  }
  _times[timeIdx] = value;
}

void Sequencer::Track::setFrequency(float frequency)
{
  _generator->setFrequency(frequency);
}

stk::StkFrames& Sequencer::Track::tick(uint64_t timeIdx)
{
  float frequency = (float)_times[timeIdx % _times.size()];
  if(frequency > 1.f) {
    //_generator->setFrequency((float)_times[timeIdx % _times.size()]);
    _generator->noteOn();
  } else {
    _generator->noteOff();
  }
  return _generator->tick();
}

stk::StkFloat Sequencer::Track::channelWeight(uint32_t channelIdx)
{
  return _generator->stereoWeight(channelIdx);
}

const stk::StkFrames& Sequencer::Track::frames() const
{
  return _generator->frames();
}

const stk::StkFloat* Sequencer::Track::samples() const
{
  return (stk::StkFloat*)&_generator->frames();
}

void Sequencer::Track::draw()
{
  _generator->draw();
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

Sequencer::~Sequencer()
{
  for(auto& track: _tracks)delete track;
  _tracks.clear();
}

void Sequencer::setLength(uint64_t length)
{
  _length = length;
  for(auto& track: _tracks) {
    track->setLength(_length);
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
  _tracks.push_back(new Track(_length));
  return _tracks.back();
}


void Sequencer::removeTrack(uint32_t trackIdx)
{
  if(_tracks.size() > trackIdx) {
    Track* track = _tracks[trackIdx];
    _tracks.erase(_tracks.begin()+trackIdx);
    delete track;
  }
}

Sequencer::Track* Sequencer::getTrack(uint32_t trackIdx)
{
  if(_tracks.size() <= trackIdx) {
    std::cerr << "invalid track index !" << std::endl;
    return NULL;
  }
  return _tracks[trackIdx];
}

void Sequencer::setTime(uint32_t trackIdx, uint64_t timeIdx, const Time& time)
{
  if(_tracks.size() <= trackIdx) {
    std::cerr << "Invalid track index : " << trackIdx << 
    " (max = " << (_tracks.size() - 1) << ")" << std::endl;
    return;
  }
  if(_length <= timeIdx) {
    std::cerr << "Invalid time index : " << timeIdx << 
    " (max = " << (_length - 1) << ")" << std::endl;
    return;
  }
  std::cout << "set time : " << timeIdx << ": " << (int)time << std::endl;
  _tracks[trackIdx]->setTime(timeIdx, time);
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

stk::StkFrames& Sequencer::tick(stk::StkFrames& frames)
{
  if(!_running) return frames;
  memset(&frames[0], 0, frames.size() * sizeof(stk::StkFloat));
  stk::StkFloat* samples = &frames[0];
  
  uint64_t timeIdx = (uint64_t) _time;

  
  for(auto& track: _tracks) {
    stk::StkFloat channelWeights[TX_NUM_CHANNELS];
    for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
      channelWeights[n] = track->channelWeight(n);
      std::cout << "channel " << n << " weight: " << channelWeights[n] << std::endl;
    }
    const stk::StkFrames& cFrames = track->tick(timeIdx % _length);
    for(size_t frameIdx = 0; frameIdx < stk::RT_BUFFER_SIZE;  ++frameIdx) {
      for(size_t channelIdx = 0; channelIdx < TX_NUM_CHANNELS; ++channelIdx) {
        *samples++ += cFrames[frameIdx] * channelWeights[channelIdx];
      }
    }
  }
  
  const float sampleTime = computeSampleTime();
  _time += 60.f / _bpm * sampleTime;

  return frames;
}