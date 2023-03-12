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
  , _frequency(110.f)
  , _harmonics(3)
{
  _inputs.push_back(new TxParameterFloat("Frequency", 20.f, 3000.f, _frequency));
  _inputs.push_back(new TxParameterFloat("Harmonics", 0, 16, _harmonics));
  //_inputs[1]->connect(_lfo);
  setWaveForm(BLIT);
}

TxGenerator::~TxGenerator() 
{
  if(_lfo) delete _lfo;
  if(_generator) delete _generator;
}

stk::StkFloat TxGenerator::tick(void)
{
  if(!_generator || !_active || !_dirty) {
    return 0.f;
  }
  //_dirty = false;
  setFrequency(_inputs[0]->tick());
  setHarmonics(_inputs[1]->tick());
  
  switch(_waveFormIdx) {
    case BLIT:
      return ((stk::Blit*)_generator)->tick();

    case BLITSAW:
      return ((stk::BlitSaw*)_generator)->tick();

    case BLITSQUARE:
      return ((stk::BlitSquare*)_generator)->tick();

    case NOISE:
      return ((stk::Noise*)_generator)->tick();

    case SINEWAVE:
      return ((stk::SineWave*)_generator)->tick();

    case SINGWAVE:
      return ((stk::SingWave*)_generator)->tick();
  }
  return 0.f;
}

stk::StkFrames& TxGenerator::tick(stk::StkFrames& frames, unsigned int channel)
{
  if(!_generator || !_active || !_dirty) {
    return frames;
  }
  //_dirty = false;
  setFrequency(_inputs[0]->tick());
  setHarmonics(_inputs[1]->tick());
  _generator->tick(frames, 0);
  for(size_t f = 0; f < frames.size(); ++f)
    frames[f] *= _volume;
  return frames;
}

void TxGenerator::setWaveForm(int8_t index)
{
  if(index == _waveFormIdx)return;
  if(_generator) delete _generator;

  switch(index % 6) {
    case BLIT:
      _generator = new stk::Blit();
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      ((stk::Blit*)_generator)->setHarmonics(_harmonics);
      ((stk::Blit*)_generator)->reset();
      break;

    case BLITSAW:
      _generator = new stk::BlitSaw();
      ((stk::BlitSaw*)_generator)->setFrequency(_frequency);
      ((stk::BlitSaw*)_generator)->setHarmonics(_harmonics);
      ((stk::BlitSaw*)_generator)->reset();
      break;

    case BLITSQUARE:
      _generator = new stk::BlitSquare();
      ((stk::BlitSquare*)_generator)->setFrequency(_frequency);
      ((stk::BlitSquare*)_generator)->setHarmonics(_harmonics);
      ((stk::BlitSquare*)_generator)->reset();
      break;

    case NOISE:
      _generator = new stk::Noise();
      break;

    case SINEWAVE:
      _generator = new stk::SineWave();
      ((stk::SineWave*)_generator)->setFrequency(_frequency);
      ((stk::SineWave*)_generator)->reset();
      break;

    case SINGWAVE:
      _generator = new stk::SingWave( (stk::Stk::rawwavePath() + "impuls20.raw").c_str(), true );
      ((stk::SingWave*)_generator)->setFrequency(_frequency);
      ((stk::SingWave*)_generator)->reset();
      break;
  }

  _waveFormIdx = index % 6;
}

void TxGenerator::setFrequency(const stk::StkFloat& frequency)
{
  if(_frequency == frequency) return;
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
  if(harmonics == _harmonics) return;
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
  
  float frequency = _frequency;  
  if (ImGuiKnobs::Knob("Frequency", &frequency, 20.f, 2000.f, 1.f, "%.1fHz", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    _inputs[0]->set(frequency);
    //setFrequency(frequency);
  }
  ImGui::SameLine();

  int harmonics = _harmonics;
  if (ImGuiKnobs::KnobInt("Harmonics", &harmonics, 1, 9, 0.1, "%i", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    _inputs[1]->set(harmonics);
    //setHarmonics(_harmonics);
  }
  ImGui::SameLine();    
  
  ImGui::BeginGroup();
  if(ImGui::Checkbox("Active", &_active)) {
    //if(!_active) clearSamples();
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