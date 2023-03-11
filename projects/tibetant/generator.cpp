#include "common.h"
#include "generator.h"
#include <Blit.h>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <Noise.h>
#include <SineWave.h>
#include <SingWave.h>


TxGenerator::TxGenerator() 
  : _generator(NULL)
  , _waveFormIdx(-1)
  , _frequency(220)
  , _frames(stk::StkFrames(stk::RT_BUFFER_SIZE, 1))
  , _name(computeHiddenName("TxGenerator", this))
{
  setWaveForm(BLIT);
  _envelope.setAllTimes(0.01, 0.005, 0.1, 0.05);
  _lfo.setFrequency(2.f);
}

TxGenerator::~TxGenerator() 
{
  if(_generator)delete _generator;
}

stk::StkFrames& TxGenerator::tick()
{
  if(!_generator) {
    return _frames;
  }
  _generator->tick(_frames, 0);
  /*
  stk::StkFloat* samples = &_frames[0];
  for(size_t i = 0; i < _frames.size(); ++i, ++samples) {
    *samples += _lfo.tick();
    *samples *= _envelope.tick();
  }
  */
  return _frames;
}

const stk::StkFrames& TxGenerator::frames() const
{
  return _frames;
}

stk::StkFloat TxGenerator::stereoWeight(uint32_t channelIdx)
{
  if(_stereo == 0.f) return 0.5f;

  const stk::StkFloat nrm = _stereo * 0.5f + 0.5f;
  if(channelIdx == 0) {
    return 1.f - nrm;
  } else {
    return nrm;
  }
}

void TxGenerator::setWaveForm(int8_t index)
{
  if(index == _waveFormIdx)return;
  if(_generator) delete _generator;

  switch(index % 6) {
    case BLIT:
      _generator = new stk::Blit();
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      ((stk::Blit*)_generator)->setHarmonics(3);
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

void TxGenerator::setStereo(const stk::StkFloat& stereo)
{
  _stereo = stereo;
}

void TxGenerator::noteOn()
{
  _envelope.keyOn();
}

void TxGenerator::noteOff()
{
  _envelope.keyOff();
}

void TxGenerator::draw()
{
  ImGui::Begin(_name.c_str(), NULL);
  
  if (ImGuiKnobs::Knob("Frequency", &_frequency, 20.f, 2000.f, 1.f, "%.1fHz", ImGuiKnobVariant_Tick, 0.f, ImGuiKnobFlags_DragHorizontal)) {
    setFrequency(_frequency);
  }
  ImGui::SameLine();
  
  ImGui::BeginGroup();
  if (ImGui::BeginCombo("Signal", TX_SIGNAL_NAME[_waveFormIdx], ImGuiComboFlags_NoArrowButton))
  {
    for (int n = 0; n < TX_NUM_SIGNAL; ++n)
    {
      const bool is_selected = (_waveFormIdx == n);
      if (ImGui::Selectable(TX_SIGNAL_NAME[n], is_selected)) {
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