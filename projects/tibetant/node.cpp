#include "common.h" 
#include "node.h"

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _nChannels(numChannels)
  , _name(name)
  , _active(true)
  , _dirty(true)
  , _volume(1.f)
{
  _frames.resize(1, 1, 0.0);

  _params.push_back(new TxParameterSamples("Samples"));
  _params.push_back(new TxParameterBool("Active", &_active));
  _params.push_back(new TxParameterFloat("Volume", 0.f, 2.f, &_volume, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat("Stereo", -1.f, 1.f, &_stereo, TxParameter::KNOB));
}

TxNode::~TxNode()
{
  for(auto& param: _params){
    delete param;
  }
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

void TxNode::setStereo(stk::StkFloat stereo) 
{
  _stereo = stereo;
};

void TxNode::setVolume(stk::StkFloat volume) 
{
  _volume = volume;
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

TxConnexion* TxNode::connect(TxNode* node, const std::string& name, short channel) 
{
  TxParameter* parameter = getParameter(name);
  if(parameter) {
    parameter->connect(node, channel);
    std::cout << "connect parameter : " << name << std::endl;
    return new TxConnexion({node, this, parameter});
  }
  return NULL;
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

void TxNode::commonControls()
{
  ImGui::BeginGroup();
  TxParameter* active = _params[ACTIVE];
  active->draw();

  TxParameter* volume = _params[VOLUME];
  volume->draw();
  
  ImGui::SameLine();
  TxParameter* stereo = _params[STEREO];
  stereo->draw();
  
  ImGui::EndGroup();
}