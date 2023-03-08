#include "generator.h"
#include <Blit.h>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <Noise.h>
#include <SineWave.h>
#include <SingWave.h>

WaveGenerator::WaveGenerator() 
  : _generator(NULL)
  , _waveFormIdx(-1)
  , _envelopeIdx(-1)
  , _frequency(220)
{
  setWaveForm(0);
}

stk::StkFloat WaveGenerator::tick()
{
  if(!_generator)return 0.f;
  return (this->*_tickImpl)();
};

void WaveGenerator::setWaveForm(int8_t index)
{
  if(index == _waveFormIdx)return;
  if(_generator) delete _generator;

  switch(index % 6) {
    case BLIT:
      _generator = new stk::Blit();
      ((stk::Blit*)_generator)->setFrequency(_frequency);
      _tickImpl = &WaveGenerator::_blitTick;
      break;

    case BLITSAW:
      _generator = new stk::BlitSaw();
      ((stk::BlitSaw*)_generator)->setFrequency(_frequency);
      _tickImpl = &WaveGenerator::_blitSawTick;
      break;

    case BLITSQUARE:
      _generator = new stk::BlitSquare();
      ((stk::BlitSquare*)_generator)->setFrequency(_frequency);
      _tickImpl = &WaveGenerator::_blitSquareTick;
      break;

    case NOISE:
      _generator = new stk::Noise();
      _tickImpl = &WaveGenerator::_noiseTick;
      break;

    case SINEWAVE:
      _generator = new stk::SineWave();
      ((stk::SineWave*)_generator)->setFrequency(_frequency);
      _tickImpl = &WaveGenerator::_sineWaveTick;
      break;

    case SINGWAVE:
      _generator = new stk::SingWave( (stk::Stk::rawwavePath() + "impuls20.raw").c_str(), true );
      ((stk::SingWave*)_generator)->setFrequency(_frequency);
      _tickImpl = &WaveGenerator::_singWaveTick;
      break;

  }
  _waveFormIdx = index % 6;
  std::cout << "waveform index " << _waveFormIdx << std::endl;
}

void WaveGenerator::setFrequency(float frequency)
{
  if(std::abs(frequency - _frequency) < 0.0001)return;
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

stk::StkFloat WaveGenerator::_blitTick()
{
  return ((stk::Blit*)_generator)->tick();
}

stk::StkFloat WaveGenerator::_blitSawTick()
{
  return ((stk::BlitSaw*)_generator)->tick();
}

stk::StkFloat WaveGenerator::_blitSquareTick()
{
  return ((stk::BlitSquare*)_generator)->tick();
}

stk::StkFloat WaveGenerator::_noiseTick()
{
  return ((stk::Noise*)_generator)->tick();
}

stk::StkFloat WaveGenerator::_sineWaveTick()
{
  return ((stk::SineWave*)_generator)->tick();
}

stk::StkFloat WaveGenerator::_singWaveTick()
{
  return ((stk::SingWave*)_generator)->tick();
}