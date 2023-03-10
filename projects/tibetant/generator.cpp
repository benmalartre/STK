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
  , _envelopeIdx(-1)
  , _frequency(220)
  , _frames(stk::StkFrames(stk::RT_BUFFER_SIZE, 1))
{
  setWaveForm(2);
  _envelope.setAllTimes(0.01, 0.005, 0.1, 0.05);
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
  
  stk::StkFloat* samples = &_frames[0];
  for(size_t i = 0; i < _frames.size(); ++i) {
    *samples++ *= _envelope.tick();
  }
  
  return _frames;
}

const stk::StkFrames& TxGenerator::frames() const
{
  return _frames;
}

void TxGenerator::setWaveForm(int8_t index)
{
  if(index == _waveFormIdx)return;
  std::cout << "set wave form" << _generator << std::endl;
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

void TxGenerator::setFrequency(float frequency)
{
  if(frequency == _frequency)return;
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

void TxGenerator::noteOn()
{
  _envelope.keyOn();
}

void TxGenerator::noteOff()
{
  _envelope.keyOff();
}