#include <Stk.h>
#include <Generator.h>
#include <ADSR.h>

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

enum SIGNAL_TYPE {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE,
    SINGWAVE
};

static const char* SIGNAL_NAME[6] = {
  "Blit",
  "BliSaw",
  "BlitSquare",
  "Noise",
  "SineWave",
  "SingWave"
};

class WaveGenerator {
public:
  WaveGenerator();
  stk::StkFloat tick();
  void setWaveForm(int8_t index);
  void setFrequency(float frequency);

private:
  stk::StkFloat (WaveGenerator::*_tickImpl)();

  stk::StkFloat _blitTick();
  stk::StkFloat _blitSawTick();
  stk::StkFloat _blitSquareTick();
  stk::StkFloat _noiseTick();
  stk::StkFloat _sineWaveTick();
  stk::StkFloat _singWaveTick();

  stk::Generator* _generator;
  stk::ADSR       _envelope;
  stk::StkFloat   _frequency;
  int8_t          _waveFormIdx;
  int8_t          _envelopeIdx;
};

#endif // TX_GENERATOR_H