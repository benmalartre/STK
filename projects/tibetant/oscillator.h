#include <Stk.h>
#include <Generator.h>
#include "node.h"

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

class TxOscillator : public TxNode {
public:
  static const int NumWaveForm = 6;
  static const char* WaveFormName[NumWaveForm];

  enum Type {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE,
    SINGWAVE
  };

  enum Parameters {
    WAVEFORM = TxNode::LAST,
    FREQUENCY,
    HARMONICS, 
    LAST
  };

  TxOscillator(const std::string& name);
  ~TxOscillator();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setWaveForm(short index);
  void setFrequency(const stk::StkFloat& frequency);
  void setHarmonics(int harmonics);

  void draw() override;

private:
  stk::Generator* _generator;
  stk::StkFloat   _frequency;

  int             _lastHarmonics;
  int             _harmonics;
  int             _lastWaveFormIdx;
  int             _waveFormIdx;
  
};

#endif // TX_GENERATOR_H