#include "common.h"
#include "oscillator.h"
#include "graph.h"
#include "editor.h"
#include <Blit.h>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <Noise.h>
#include <SineWave.h>
#include <Modulate.h>
#include <Granulate.h>

const char* TxOscillator::WaveFormName[TxOscillator::NumWaveForm] = {
  "Blit",
  "BlitSaw",
  "BlitSquare",
  "Noise",
  "SineWave"
};

TxOscillator::TxOscillator(TxNode* parent, const std::string& name) 
  : TxNode(parent, TxNode::OSCILLATOR, name)
  , _generator(NULL)
  , _waveFormIdx(-1)
  , _lastWaveFormIdx(-1)
  , _envelope(1.f)
  , _frequency(110.f)
  , _lastHarmonics(3)
  , _harmonics(3)
{
  setWaveForm(BLIT);
  _params.push_back(new TxParameterEnum(this, "Wave", 
    &TxOscillator::WaveFormName[0], TxOscillator::NumWaveForm, &_waveFormIdx, WAVEFORM));
  _params.push_back(new TxParameterFloat(this, "Freq", 20.f, 3000.f, 
    &_frequency, FREQUENCY, TxParameter::KNOB));
  _params.push_back(new TxParameterInt(this, "Harm", 0, 16, 
    &_harmonics, HARMONICS, TxParameter::KNOB));
  _params.push_back(new TxParameterFloat(this, "Env", 0.f, 2.f, 
    &_envelope, ENVELOPE, TxParameter::KNOB));
  _size = ImVec2(TX_KNOB_MIDDLE_SIZE * 3 + TX_PADDING_X * 4, TX_PADDING_Y * 2 + TX_KNOB_MIDDLE_SIZE * 2 + TX_SLIDER_SIZE);
}

TxOscillator::~TxOscillator() 
{
  if(_generator) delete _generator;
}

const ImVec2& TxOscillator::size()
{
  return _size;
}

stk::StkFloat TxOscillator::tick(unsigned int channel)
{
  if(!_generator)
    return 0.f;
  
  if(!_dirty)
    return _frames[channel];

  if(_lastWaveFormIdx != (int)_params[WAVEFORM]->tick())
    setWaveForm(_waveFormIdx);

  _envelope = _params[ENVELOPE]->tick();
  setFrequency(_params[FREQUENCY]->tick());
  setHarmonics(_params[HARMONICS]->tick());
  _frames[channel] = 0.f;
  switch(_waveFormIdx) {
    case BLIT:
      _frames[channel] = ((stk::Blit*)_generator)->tick() * _envelope;
      break;

    case BLITSAW:
      _frames[channel] = ((stk::BlitSaw*)_generator)->tick() * _envelope;
      break;

    case BLITSQUARE:
      _frames[channel] = ((stk::BlitSquare*)_generator)->tick() * _envelope;
      break;

    case NOISE:
      _frames[channel] = ((stk::Noise*)_generator)->tick() * _envelope;
      break;

    case SINEWAVE:
      _frames[channel] = ((stk::SineWave*)_generator)->tick() * _envelope;
      break;
  }   
  _buffer.write(_frames[channel]);
  _dirty = false;
  return _frames[channel];
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

  }
}