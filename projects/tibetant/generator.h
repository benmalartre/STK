#include <Stk.h>
#include <Generator.h>
#include "node.h"

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

#define TX_NUM_SIGNAL 6

class TxGenerator : public TxNode {
public:
  enum Type {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE,
    SINGWAVE
  };

  static const int NumWaveForm = 6;
  static const char* WaveFormName[NumWaveForm];

  enum Parameters {
    WAVEFORM = TxNode::LAST,
    FREQUENCY,
    HARMONICS, 
    LAST
  };

  TxGenerator(const std::string& name);
  ~TxGenerator();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setWaveForm(int8_t index);
  void setFrequency(const stk::StkFloat& frequency);
  void setHarmonics(int harmonics);

  void draw() override;
  void hello(){std::cout << "hello ben :)" << std::endl;};

private:
  stk::Generator* _generator;
  stk::StkFloat   _frequency;

  int             _lastHarmonics;
  int             _harmonics;
  int             _lastWaveFormIdx;
  int             _waveFormIdx;
  
};

// callbacks
static void setHarmonicsCallback(void* ctxt) 
{
  static_cast<TxGenerator*>(ctxt)->hello();
}

#endif // TX_GENERATOR_H