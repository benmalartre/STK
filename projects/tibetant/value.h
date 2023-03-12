#include "node.h"

#ifndef TX_VALUE_H
#define TX_VALUE_H

class TxValue : public TxNode {
public:
  TxValue(const std::string& name);
  ~TxValue();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
  void setValue(const stk::StkFloat& value);
  void draw() override;
private:
  stk::StkFloat  _value;
};

#endif // TX_VALUE_H