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
  TxRandom(TxNode* parent, const std::string& name);
  ~TxRandom();
  const ImVec2& size();
  stk::StkFloat tick(unsigned int) override;
  void setMinimum(stk::StkFloat value);
  void setMaximum(stk::StkFloat value);
  void setSeed(int seed);
  void setFrequency(stk::StkFloat frequency);
  void reset() override;
  void updateBounds();

protected:
  void _drawImpl(TxEditor*, bool*) override;

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