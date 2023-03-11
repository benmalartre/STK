#include "common.h"
#include "generator.h"
#include <Blit.h>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <Noise.h>
#include <SineWave.h>
#include <SingWave.h>


TxGenerator::TxGenerator(const std::string& name) 
  : TxNode(name)
  , _generator(NULL)
  , _lfo(new TxLfo(name + "_lfo"))
  , _waveFormIdx(-1)
  , _frequency(220.f)
  , _harmonics(3)
{
  _inputs.push_back(TxParameter("Frequency", 20.f, 3000.f, _frequency));
  _inputs[0].connect(_lfo);
  setWaveForm(BLIT);
  setHarmonics(_harmonics);
}

TxGenerator::~TxGenerator() 
{
  if(_lfo) delete _lfo;
  if(_generator) delete _generator;
}

stk::StkFrames& TxGenerator::tick()
{
  if(!_generator || !_active || !_dirty) {
    return _frames;
  }
  //_dirty = false;
  setFrequency(_inputs[0].tick());
  _generator->tick(_frames, 0);
  for(size_t f = 0; f < _frames.size(); ++f)
    _frames[f] *= _volume;
  return _frames;
}

void TxGenerator::setWaveForm(int8_t index)
{
  if(index == _waveFormIdx)return;
  if(_generator) delete _generator;

  switch(index % 6) {
    case BLIT:
      _generator = new stk::Blit();
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      break;

    case BLITSAW:
      _generator = new stk::BlitSaw();
      ((stk::BlitSaw*)_generator)->setFrequency(_frequency);
      break;

    case BLITSQUARE:
      _generator = new stk::BlitSquare();
      ((stk::BlitSquare*)_generator)->setFrequency(_frequency);
      break;

    case NOISE:
      _generator = new stk::Noise();
      break;

    case SINEWAVE:
      _generator = new stk::SineWave();
      ((stk::SineWave*)_generator)->setFrequency(_frequency);
      break;

    case SINGWAVE:
      _generator = new stk::SingWave( (stk::Stk::rawwavePath() + "impuls20.raw").c_str(), true );
      ((stk::SingWave*)_generator)->setFrequency(_frequency);
      break;

  }
  _waveFormIdx = index % 6;
}

void TxGenerator::setFrequency(const stk::StkFloat& frequency)
{
  _frequency = frequency;

  switch(_waveFormIdx) {
    case BLIT:
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      break;

    case BLITSAW:
      ((stk::BlitSaw*)_generator)->setFrequency(_frequency);
      break;

    case BLITSQUARE:
      ((stk::BlitSquare*)_generator)->setFrequency(_frequency);
      break;

    case NOISE:
      break;

    case SINEWAVE:
      ((stk::SineWave*)_generator)->setFrequency(_frequency);
      break;

    case SINGWAVE:
      ((stk::SingWave*)_generator)->setFrequency(_frequency);
      break;
  }
}

void TxGenerator::setHarmonics(int harmonics)
{
  _harmonics = harmonics;

  switch(_waveFormIdx) {
    case BLIT:
      ((stk::Blit*)_generator)->setHarmonics(_harmonics);
      break;

    case BLITSAW:
      ((stk::BlitSaw*)_generator)->setHarmonics(_harmonics);
      break;

    case BLITSQUARE:
      ((stk::BlitSquare*)_generator)->setHarmonics(_harmonics);
      break;

    case NOISE:
    case SINEWAVE:
    case SINGWAVE:
      break;
  }
}

void TxGenerator::draw()
{
  ImGui::Begin(_name.c_str(), NULL);
  
  if (ImGuiKnobs::Knob("Frequency", &_frequency, 20.f, 2000.f, 1.f, "%.1fHz", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    setFrequency(_frequency);
  }
  ImGui::SameLine();

  if (ImGuiKnobs::KnobInt("Harmonics", &_harmonics, 1, 9, 1, "%i", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    setHarmonics(_harmonics);
  }
  ImGui::SameLine();    
  
  ImGui::BeginGroup();
  if(ImGui::Checkbox("Active", &_active)) {
    if(!_active) clearSamples();
  }
  ImGui::SliderFloat("Volume", &_volume, 0.f, 2.f);
  if (ImGui::BeginCombo("Signal", TX_WAVEFORM_NAME[_waveFormIdx], ImGuiComboFlags_NoArrowButton))
  {
    for (int n = 0; n < TX_NUM_SIGNAL; ++n)
    {
      const bool is_selected = (_waveFormIdx == n);
      if (ImGui::Selectable(TX_WAVEFORM_NAME[n], is_selected)) {
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
  ImGui::End();
}