#include "mixer.h"


TxMixer::TxMixer()
  : TxNode("TxMixer", TX_NUM_CHANNELS)
  , _bpm(60)
  , _n(4)
  , _length(8)
  , _time(0.0)
  , _running(false)
{
}

TxMixer::TxMixer(uint32_t bpm, uint32_t n, uint64_t length)
  : TxNode("TxMixer", TX_NUM_CHANNELS)
  , _bpm(bpm)
  , _n(n)
  , _length(length)
  , _time(0.0)
  , _running(false)
{
}

TxMixer::~TxMixer()
{

}

void TxMixer::setLength(uint64_t length)
{
  _length = length;
  _sequence.resize(_length + _n);
}

void TxMixer::setBPM(uint32_t bpm)
{
  _bpm = bpm;
}

uint32_t TxMixer::numTracks()
{
  return _tracks.size();
}

TxMixer::Track* TxMixer::addTrack(const std::string& name)
{
  _tracks.push_back(new Track(name, _length));
  return _tracks.back();
}


void TxMixer::removeTrack(uint32_t trackIdx)
{
  if(_tracks.size() > trackIdx) {
    Track* track = _tracks[trackIdx];
    _tracks.erase(_tracks.begin()+trackIdx);
    delete track;
  }
}

TxMixer::Track* TxMixer::getTrack(uint32_t trackIdx)
{
  if(_tracks.size() <= trackIdx) {
    std::cerr << "invalid track index !" << std::endl;
    return NULL;
  }
  return _tracks[trackIdx];
}

void TxMixer::setTime(uint32_t trackIdx, uint64_t timeIdx, const Time& time)
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
  _tracks[trackIdx]->setTime(timeIdx, time);
}

void TxMixer::start()
{
  _running = true;
  _time = 0;
}

void TxMixer::stop()
{
  _running = false;
}


stk::StkFloat TxMixer::tick(void)
{
  return RANDOM_LO_HI(-1.f, 1.f);
}

stk::StkFloat TxMixer::tick(uint32_t trackIdx)
{
  return RANDOM_LO_HI(-1.f, 1.f);
}

stk::StkFrames& TxMixer::tick(stk::StkFrames& frames, unsigned int channel)
{
  memset(&frames[0], 0, frames.size() * sizeof(stk::StkFloat));
  if(!_active) return frames;
  
  uint64_t timeIdx = (uint64_t) _time;

  for(auto& track: _tracks) {
    stk::StkFloat* samples = &frames[0];
    stk::StkFloat channelWeights[TX_NUM_CHANNELS];
    for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
      channelWeights[n] = track->getNode()->stereoWeight(n);
    }
    //const stk::StkFloat sample = track->tick(timeIdx % _length);
    for(size_t frameIdx = 0; frameIdx < stk::RT_BUFFER_SIZE;  ++frameIdx) {
      for(size_t channelIdx = 0; channelIdx < TX_NUM_CHANNELS; ++channelIdx) {
        *samples++ += track->tick(timeIdx % _length) * channelWeights[channelIdx];
      }
    }
  }
  
  const float sampleTime = computeSampleTime();
  _time += 60.f / _bpm * sampleTime;

  return frames;
}

void TxMixer::draw()
{
  ImGui::Begin(_name.c_str(), NULL);
  
  commonControls();
  for(auto& track: _tracks) {
    //track->draw();
  } 

  ImGui::End();
}

