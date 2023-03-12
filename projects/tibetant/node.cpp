#include "node.h"

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _nChannels(numChannels)
  , _name(name)
  , _active(true)
  , _dirty(true)
  , _volume(1.f)
{
}

TxNode::~TxNode()
{
}

int TxNode::numChannels()
{
  return _nChannels;
}

void TxNode::setActive(bool state) 
{
    _active = state;
};

void TxNode::setDirty(bool state) 
{
    _dirty = state;
};

stk::StkFloat TxNode::lastSample(unsigned int channel)
{
  return _sample[channel];
}

void TxNode::setStereo(const stk::StkFloat& stereo) 
{
    _stereo = stereo;
};

stk::StkFloat TxNode::stereoWeight(uint32_t channelIdx)
{
  if(_stereo == 0.f) return 1.f;

  const stk::StkFloat nrm = _stereo * 0.5f + 0.5f;
  if(channelIdx == 0) {
    return 2.f * (1.f - nrm);
  } else {
    return 2.f * nrm;
  }
}

bool TxNode::connect(TxNode* node, const std::string& parameter) 
{
  for(auto& input: _inputs) {
    if(input->name() == parameter) {

    }
  }
  return false;
}