#include "node.h"

TxParameter::TxParameter(const std::string& name, const stk::StkFloat& minimum, 
  const stk::StkFloat& maximum, const stk::StkFloat& value, TxNode* input)
  : _name(name)
  , _minimum(minimum)
  , _maximum(maximum)
  , _value(value)
  , _input(input)
{
}

const std::string& TxParameter::name()
{
  return _name;
};

void TxParameter::connect(TxNode* node)
{
  _input = node;
}

void TxParameter::disconnect()
{
  _input = NULL;
}

stk::StkFloat TxParameter::tick()
{
  if(_input) {
    _input->tick();
    return _input->lastSample();
  }
  return _value;
}

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _frames(stk::StkFrames(stk::RT_BUFFER_SIZE, numChannels))
  , _name(name)
  , _active(true)
  , _dirty(true)
  , _volume(1.f)
{
}

TxNode::~TxNode()
{
}

const stk::StkFrames& TxNode::frames() const 
{
  return _frames;
};

void TxNode::setActive(bool state) 
{
    _active = state;
};

void TxNode::setDirty(bool state) 
{
    _dirty = state;
};

void TxNode::clearSamples()
{
  memset(&_frames[0], 0, _frames.size() * sizeof(stk::StkFloat));
}

stk::StkFloat TxNode::lastSample()
{
  return _frames[_frames.size()-1];
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
    if(input.name() == parameter) {

    }
  }
  return false;
}

TxValue::TxValue(const std::string& name)
  : TxNode(name)
{
  
}

TxValue::~TxValue()
{

}

void TxValue::setValue(const stk::StkFloat& value)
{
  _value = value;
}

stk::StkFrames& TxValue::tick()
{
  for(size_t i = 0; i < _frames.size(); ++i) {
    _frames[i] = _value;
  }
  return _frames;
}

void TxValue::draw()
{

}