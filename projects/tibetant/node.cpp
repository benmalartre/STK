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
  _params.push_back(new TxParameterFloat("Volume", 0.f, 2.f, &_volume));
  _params.push_back(new TxParameterFloat("Stereo", -1.f, 1.f, &_stereo));
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

void TxNode::draw()
{
  ImGui::Begin(_name.c_str(), NULL);
  
  for(auto& param: _params) {
    param->draw();
  }
  /*
  float frequency = _frequency;  
  if (ImGuiKnobs::Knob("Frequency", &frequency, 20.f, 2000.f, 1.f, "%.1fHz", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    _params[]->set(frequency);
    //setFrequency(frequency);
  }
  ImGui::SameLine();

  int harmonics = _harmonics;
  if (ImGuiKnobs::KnobInt("Harmonics", &harmonics, 1, 9, 0.1, "%i", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    _params[1]->set(harmonics);
    //setHarmonics(_harmonics);
  }
  ImGui::SameLine();    
  
  ImGui::BeginGroup();
  if(ImGui::Checkbox("Active", &_active)) {
    //if(!_active) clearSamples();
  }
  ImGui::SliderFloat("Volume", &_volume, 0.f, 2.f);
  if (ImGui::BeginCombo("Signal", TxGenerator::WaveFormName[_waveFormIdx], ImGuiComboFlags_NoArrowButton))
  {
    for (int n = 0; n < TX_NUM_SIGNAL; ++n)
    {
      const bool is_selected = (_waveFormIdx == n);
      if (ImGui::Selectable(TxGenerator::WaveFormName[n], is_selected)) {
        setWaveForm(n);
      }
      // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  
  if (ImGui::SliderFloat("Stereo", &_stereo, -1.f, 1.f)) {
  }

  ImGui::EndGroup();
  */
  ImGui::End();
}