#include <Stk.h>
#include <Generator.h>
#include <ADSR.h>
#include <SineWave.h>

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

#define TX_NUM_SIGNAL 6

enum TX_SIGNAL_TYPE {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE,
    SINGWAVE
};

static const char* TX_SIGNAL_NAME[6] = {
  "Blit",
  "BlitSaw",
  "BlitSquare",
  "Noise",
  "SineWave",
  "SingWave"
};

class TxGenerator {
public:
  TxGenerator();
  ~TxGenerator();
  stk::StkFrames& tick();
  const stk::StkFrames& frames() const;
  void setWaveForm(int8_t index);
  void setFrequency(const stk::StkFloat& frequency);
  void setStereo(const stk::StkFloat& stereo);
  void noteOn();
  void noteOff();
  void draw();
  stk::StkFloat stereoWeight(uint32_t channelIdx);

private:
  std::string     _name;
  stk::StkFrames  _frames;
  stk::Generator* _generator;
  stk::ADSR       _envelope;
  stk::SineWave   _lfo;
  stk::StkFloat   _frequency;
  stk::StkFloat   _stereo;
  int             _waveFormIdx;
};

#endif // TX_GENERATOR_H