#include <Stk.h>
#include <Generator.h>
#include "node.h"
#include "lfo.h"

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

#define TX_NUM_SIGNAL 6

class TxGenerator : public TxNode {
public:
  enum WaveFormType {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE,
    SINGWAVE
  };

  static const char* WaveFormName[6];

  enum Parameters {
    FREQUENCY = TxNode::LAST,
    HARMONICS = 3
  };

  TxGenerator(const std::string& name);
  ~TxGenerator();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setWaveForm(int8_t index);
  void setFrequency(const stk::StkFloat& frequency);
  void setHarmonics(int harmonics);
  void draw() override;

private:
  stk::Generator* _generator;
  TxLfo*          _lfo;
  stk::StkFloat   _frequency;
  int             _waveFormIdx;
  int             _harmonics;
};

#endif // TX_GENERATOR_H