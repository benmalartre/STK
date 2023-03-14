#include "node.h"

#ifndef TX_RANDOM_H
#define TX_RANDOM_H

class TxRandom : public TxNode {
public:
  enum Parameters {
    FREQUENCY = TxNode::LAST,
    MINIMUM,
    MAXIMUM, 
    SEED
  };
  TxRandom(const std::string& name);
  ~TxRandom();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
  void setMinimum(stk::StkFloat value);
  void setMaximum(stk::StkFloat value);
  void setSeed(int seed);
  void setFrequency(stk::StkFloat frequency);
  void draw() override;
  void updateBounds();

private:
  stk::StkFloat  _value;
  stk::StkFloat  _lastFrequency;
  stk::StkFloat  _frequency;
  stk::StkFloat  _minimum;
  stk::StkFloat  _maximum;
  int            _seed;
  int            _cnt;
  int            _rate;
};

static void updateRandomBounds(void* node)
{
  static_cast<TxRandom*>(node)->updateBounds();
}

#endif // TX_VALUE_H