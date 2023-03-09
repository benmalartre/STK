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
  stk::StkFrames& tick();
  void setWaveForm(int8_t index);
  void setFrequency(float frequency);
  void noteOn();
  void noteOff();

private:
  stk::StkFrames  _frames;
  stk::Generator* _generator;
  stk::ADSR       _envelope;
  stk::StkFloat   _frequency;
  int             _waveFormIdx;
  int             _envelopeIdx;
};

#endif // TX_GENERATOR_H