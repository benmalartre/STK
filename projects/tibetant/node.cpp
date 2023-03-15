#include "common.h" 
#include "node.h"

TxNode::TxNode(const std::string& name, uint32_t numChannels)
  : _nChannels(numChannels)
  , _name(name)
  , _active(true)
  , _dirty(true)
  , _volume(1.f)
{
  _frames.resize((int)stk::Stk::sampleRate(), 1, 0.0);

  _params.push_back(new TxParameterSamples("Samples"));
  _params.push_back(new TxParameterBool("Active", &_active));
  _params.push_back(new TxParameterFloat("Volume", 0.f, 2.f, &_volume, TxParameter::KNOB));
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

void TxNode::setVolume(stk::StkFloat volume) 
{
  _volume = volume;
};

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
  _buffer.draw();
  ImGui::BeginGroup();
  TxParameter* active = _params[ACTIVE];
  active->draw();

  TxParameter* volume = _params[VOLUME];
  volume->draw();
  
  ImGui::EndGroup();
}