#include "sequencer.h"

void Sequencer::Track::setLength(uint64_t length)
{
  _times.resize(length);
};

void Sequencer::Track::setTime(uint64_t timeIdx, const Time& value)
{
  if(timeIdx >= _times.size()){
    _times.resize(timeIdx+1);
  }
  _times[timeIdx] = value;
}

void Sequencer::Track::setNode(TxNode* node)
{
  _node = node;
}


stk::StkFloat Sequencer::Track::tick(uint64_t timeIdx)
{
  if(_node) return _node->tick();
  return 0.f;
}

void Sequencer::Track::draw()
{
  if(_node) _node->draw();
}

Sequencer::Sequencer()
  : TxNode("TxSequencer", TX_NUM_CHANNELS)
  , _bpm(60)
  , _n(4)
  , _length(8)
  , _time(0.0)
  , _running(false)
{
}

Sequencer::Sequencer(uint32_t bpm, uint32_t n, uint64_t length)
  : TxNode("TxSequencer", TX_NUM_CHANNELS)
  , _bpm(bpm)
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

Sequencer::Track* Sequencer::addTrack(const std::string& name)
{
  _tracks.push_back(new Track(name, _length));
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

stk::StkFloat Sequencer::tick(void)
{
  return RANDOM_LO_HI(-1.f, 1.f);
}

stk::StkFloat Sequencer::tick(uint32_t trackIdx)
{
  return RANDOM_LO_HI(-1.f, 1.f);
}

stk::StkFrames& Sequencer::tick(stk::StkFrames& frames, unsigned int channel)
{
  memset(&frames[0], 0, frames.size() * sizeof(stk::StkFloat));
  if(!_active) return frames;
  
  uint64_t timeIdx = (uint64_t) _time;

  for(auto& track: _tracks) {
    stk::StkFloat* samples = &frames[0];
    stk::StkFloat channelWeights[TX_NUM_CHANNELS];
    for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
      channelWeights[n] = 1.f;//track->channelWeight(n);
    }
    //const stk::StkFloat sample = track->tick(timeIdx % _length);
    for(size_t frameIdx = 0; frameIdx < stk::RT_BUFFER_SIZE;  ++frameIdx) {
      for(size_t channelIdx = 0; channelIdx < TX_NUM_CHANNELS; ++channelIdx) {
        *samples++ += track->tick(timeIdx % _length);
      }
    }
  }
  
  const float sampleTime = computeSampleTime();
  _time += 60.f / _bpm * sampleTime;

  return frames;
}
