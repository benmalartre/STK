#include <Stk.h>
#include <Generator.h>
#include "node.h"

#ifndef TX_GENERATOR_H
#define TX_GENERATOR_H

class TxOscillator : public TxNode {
public:
  static const int NumWaveForm = 5;
  static const char* WaveFormName[NumWaveForm];

  enum Type {
    BLIT,
    BLITSAW,
    BLITSQUARE,
    NOISE,
    SINEWAVE
  };

  enum Parameters {
    WAVEFORM = TxNode::LAST,
    FREQUENCY,
    HARMONICS,
    ENVELOPE,
    LAST
  };

  TxOscillator(TxNode* parent, const std::string& name);
  ~TxOscillator();
  stk::StkFloat tick(unsigned int) override;

  void setWaveForm(short index);
  void setFrequency(const stk::StkFloat& frequency);
  void setHarmonics(int harmonics);
  void setEnvelope(stk::StkFloat envelope);

  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(TxEditor*, bool*) override;

private:
  stk::Generator* _generator;
  stk::StkFloat   _frequency;
  stk::StkFloat   _envelope;

  int             _lastHarmonics;
  int             _harmonics;
  int             _lastWaveFormIdx;
  int             _waveFormIdx;
  
};

#endif // TX_GENERATOR_H