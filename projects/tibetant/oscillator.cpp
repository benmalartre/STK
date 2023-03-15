#include "common.h"
#include "oscillator.h"
#include <Blit.h>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <Noise.h>
#include <SineWave.h>
#include <SingWave.h>
#include <Modulate.h>
#include <Granulate.h>

const char* TxOscillator::WaveFormName[TxOscillator::NumWaveForm] = {
  "Blit",
  "BlitSaw",
  "BlitSquare",
  "Noise",
  "SineWave",
  "SingWave"
};

TxOscillator::TxOscillator(const std::string& name) 
  : TxNode(name)
  , _generator(NULL)
  , _waveFormIdx(-1)
  , _lastWaveFormIdx(-1)
  , _frequency(110.f)
  , _lastHarmonics(3)
  , _harmonics(3)
{
  setWaveForm(BLIT);
  _params.push_back(new TxParameterEnum("WaveForm", &TxOscillator::WaveFormName[0], 
    TxOscillator::NumWaveForm, &_waveFormIdx));
  _params.push_back(new TxParameterFloat("Frequency", 20.f, 3000.f, &_frequency, TxParameter::KNOB));
  _params.push_back(new TxParameterInt("Harmonics", 0, 16, &_harmonics, TxParameter::KNOB));
}

TxOscillator::~TxOscillator() 
{
  if(_generator) delete _generator;
}

stk::StkFloat TxOscillator::tick(unsigned int)
{
  if(!_generator || !_active || !_dirty) {
    return 0.f;
  }
  if(_lastWaveFormIdx != (int)_params[WAVEFORM]->tick()) {
    setWaveForm(_waveFormIdx);
  }
  _volume = _params[VOLUME]->tick();
  setFrequency(_params[FREQUENCY]->tick());
  setHarmonics(_params[HARMONICS]->tick());
  switch(_waveFormIdx) {
    case BLIT:
      return ((stk::Blit*)_generator)->tick() * _volume;

    case BLITSAW:
      return ((stk::BlitSaw*)_generator)->tick() * _volume;

    case BLITSQUARE:
      return ((stk::BlitSquare*)_generator)->tick() * _volume;

    case NOISE:
      return ((stk::Noise*)_generator)->tick() * _volume;

    case SINEWAVE:
      return ((stk::SineWave*)_generator)->tick() * _volume;

    case SINGWAVE:
      return ((stk::SingWave*)_generator)->tick() * _volume;
  }
  return 0.f;
}

stk::StkFrames& TxOscillator::tick(stk::StkFrames& frames, unsigned int channel)
{
  if(!_generator || !_active || !_dirty) {
    return frames;
  }

  //_dirty = false;
  if(_lastWaveFormIdx != (int)_params[WAVEFORM]->tick()) {
    setWaveForm(_waveFormIdx);
  }
  
  for(size_t f = 0; f < frames.size(); ++f) {
    setFrequency(_params[FREQUENCY]->tick());
    setHarmonics(_params[HARMONICS]->tick());
    frames[f] = tick(channel);
    frames[f] *= _volume;
  }

  return frames;
}

void TxOscillator::setWaveForm(short index)
{
  std::cout << index << "," << NumWaveForm << std::endl;
  _waveFormIdx = index % NumWaveForm;
  if(_generator) delete _generator;
  std::cout << "set wave form : " << _waveFormIdx << std::endl;

  switch(_waveFormIdx) {
    case BLIT:
      _generator = new stk::Blit();
      ((stk::Blit*)_generator)->setHarmonics(_harmonics);
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      ((stk::Blit*)_generator)->reset();
      break;

    case BLITSAW:
      _generator = new stk::BlitSaw();
      ((stk::BlitSaw*)_generator)->setHarmonics(_harmonics);
      ((stk::BlitSaw*)_generator)->setFrequency(_frequency);
      ((stk::BlitSaw*)_generator)->reset();
      break;

    case BLITSQUARE:
      _generator = new stk::BlitSquare();
      ((stk::BlitSquare*)_generator)->setHarmonics(_harmonics);
      ((stk::BlitSquare*)_generator)->setFrequency(_frequency);
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
  _lastWaveFormIdx = _waveFormIdx;
}

void TxOscillator::setFrequency(const stk::StkFloat& frequency)
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

void TxOscillator::setHarmonics(int harmonics)
{
  if(harmonics == _lastHarmonics)return;
  _lastHarmonics = harmonics;
  _harmonics = harmonics;

  switch(_waveFormIdx) {
    case BLIT:
      ((stk::Blit*)_generator)->setHarmonics(_harmonics);
      break;

    case BLITSAW:
      ((stk::BlitSaw*)_generator)->setHarmonics(_harmonics);
      //((stk::BlitSaw*)_generator)->reset();
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

void TxOscillator::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  ImGui::BeginGroup();
  ImGui::SetNextItemWidth(128);
  TxParameter* waveform = _params[TxOscillator::WAVEFORM];
  waveform->draw();
  TxParameter* frequency = _params[TxOscillator::FREQUENCY];
  frequency->draw();
  ImGui::SameLine();
  TxParameter* harmonics = _params[TxOscillator::HARMONICS];
  harmonics->draw();
  ImGui::EndGroup();

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(20, 100));
  ImGui::SameLine();
  commonControls();
}

void TxOscillator::reset()
{
  if(!_generator)return;
  switch(_waveFormIdx) {
    case BLIT:
      ((stk::Blit*)_generator)->reset();
      break;

    case BLITSAW:
      ((stk::BlitSaw*)_generator)->reset();
      break;

    case BLITSQUARE:
      ((stk::BlitSquare*)_generator)->reset();
      break;

    case NOISE:
      break;

    case SINEWAVE:
      ((stk::SineWave*)_generator)->reset();
      break;
      
    case SINGWAVE:
      ((stk::SingWave*)_generator)->reset();
      break;
  }
}