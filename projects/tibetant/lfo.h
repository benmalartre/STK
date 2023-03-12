#include <Stk.h>
#include <SineWave.h>
#include "node.h"

#ifndef TX_LFO_H
#define TX_LFO_H


class TxLfo : public TxNode {
public:
  TxLfo(const std::string& name);
  ~TxLfo();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setFrequency(const stk::StkFloat& frequency);
  void draw() override;

private:
  stk::SineWave   _sine;
  stk::StkFloat   _frequency;
  stk::StkFloat   _amplitude;
  stk::StkFloat   _offset;
};

#endif // TX_GENERATOR_H