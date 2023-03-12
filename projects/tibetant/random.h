#include "node.h"

#ifndef TX_RANDOM_H
#define TX_RANDOM_H

class TxRandom : public TxNode {
public:
  TxRandom(const std::string& name);
  ~TxRandom();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
  void setMinimum(stk::StkFloat value);
  void setMaximum(stk::StkFloat value);
  void setSeed(int seed);
  void draw() override;

private:
  stk::StkFloat  _value;
  stk::StkFloat  _minimum;
  stk::StkFloat  _maximum;
  int            _seed;
};

#endif // TX_VALUE_H