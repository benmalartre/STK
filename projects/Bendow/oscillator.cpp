#include "common.h"
#include "oscillator.h"
#include "graph.h"
#include "editor.h"
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

TxOscillator::TxOscillator(TxNode* parent, const std::string& name) 
  : TxNode(parent, TxNode::OSCILLATOR, name)
  , _generator(NULL)
  , _waveFormIdx(-1)
  , _lastWaveFormIdx(-1)
  , _frequency(110.f)
  , _lastHarmonics(3)
  , _harmonics(3)
{
  setWaveForm(BLIT);
  _params.push_back(new TxParameterEnum(this, "WaveForm", &TxOscillator::WaveFormName[0], 
    TxOscillator::NumWaveForm, &_waveFormIdx));
  _params.push_back(new TxParameterFloat(this, "Frequency", 20.f, 3000.f, &_frequency, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "Harmonics", 0, 16, &_harmonics, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Envelope", 0.f, 2.f, &_envelope, TxParameter::KNOB));
  _size = ImVec2(TX_KNOB_SIZE * 4 + TX_PADDING_X * 2, TX_PADDING_Y * 2 + TX_KNOB_SIZE * 2 + TX_SLIDER_SIZE);
}

TxOscillator::~TxOscillator() 
{
  if(_generator) delete _generator;
}

const ImVec2& TxOscillator::size()
{
  return _size;
}

stk::StkFloat TxOscillator::tick(unsigned int)
{
  if(!_generator || !_dirty) {
    return 0.f;
  }
  if(_lastWaveFormIdx != (int)_params[WAVEFORM]->tick()) {
    setWaveForm(_waveFormIdx);
  }
  _envelope = _params[ENVELOPE]->tick();
  setFrequency(_params[FREQUENCY]->tick());
  setHarmonics(_params[HARMONICS]->tick());
  float sample = 0.f;
  switch(_waveFormIdx) {
    case BLIT:
      sample = ((stk::Blit*)_generator)->tick() * _envelope;
      break;

    case BLITSAW:
      sample = ((stk::BlitSaw*)_generator)->tick() * _envelope;
      break;

    case BLITSQUARE:
      sample = ((stk::BlitSquare*)_generator)->tick() * _envelope;
      break;

    case NOISE:
      sample = ((stk::Noise*)_generator)->tick() * _envelope;
      break;

    case SINEWAVE:
      sample = ((stk::SineWave*)_generator)->tick() * _envelope;
      break;

    case SINGWAVE:
      sample = ((stk::SingWave*)_generator)->tick() * _envelope;
      break;
  }   
  //_buffer.write(sample);
  return sample;
}

void TxOscillator::setWaveForm(short index)
{
  _waveFormIdx = index % NumWaveForm;
  if(_generator) delete _generator;

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
      break;

    case BLITSQUARE:
      ((stk::BlitSquare*)_generator)->setHarmonics(_harmonics);
      break;

    case NOISE:
      break;

    case SINEWAVE:
      break;

    case SINGWAVE:
      break;
  }
}

void TxOscillator::_drawImpl(TxEditor* editor, bool* modified)
{  
  ImGui::BeginGroup();
  
  TxNode::_drawAlignLeft(editor);
  TxParameter* frequency = _params[TxOscillator::FREQUENCY];
  if(frequency->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* harmonics = _params[TxOscillator::HARMONICS];
  if(harmonics->draw(editor) && modified)*modified = true;
  ImGui::SameLine();
  TxParameter* envelope = _params[TxOscillator::ENVELOPE];
  if(envelope->draw(editor) && modified)*modified = true;

  ImGui::SetNextItemWidth(128 * editor->scale());
  TxParameter* waveform = _params[TxOscillator::WAVEFORM];
  if(waveform->draw(editor) && modified)*modified = true;
  
  ImGui::EndGroup();

  TxNode::_drawInput(editor, _params[TxOscillator::FREQUENCY], 0);
  TxNode::_drawInput(editor, _params[TxOscillator::HARMONICS], 1);
  TxNode::_drawInput(editor, _params[TxOscillator::ENVELOPE], 2);
  TxNode::_drawInput(editor, _params[TxOscillator::WAVEFORM], 3);

  TxNode::_drawOutput(editor);
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