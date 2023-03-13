#include "node.h"

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _nChannels(numChannels)
  , _name(name)
  , _active(true)
  , _dirty(true)
  , _volume(1.f)
{
  _frames.resize(1, 1, 0.0);
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
  return _frames[channel];
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

bool TxNode::connect(TxNode* node, const std::string& name) 
{
  TxParameter* parameter = getParameter(name);
  if(parameter) {
    parameter->connect(node);
    return true;
  }
  return false;
}

void TxNode::disconnect(const std::string& name) 
{
  TxParameter* parameter = getParameter(name);
  if(parameter) {
    parameter->disconnect();
  }
}

TxParameter* TxNode::getParameter(const std::string& name)
{
  for(auto& param: _params) {
    if(param->name() == name) {
      return param;
    }
  }
  return NULL;
}