#include "node.h"

#ifndef TX_VALUE_H
#define TX_VALUE_H

class TxValue : public TxNode {
public:
  TxValue(TxGraph* parent, const std::string& name);
  ~TxValue();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
  void setValue(const stk::StkFloat& value);
  void reset() override;

protected:
  void _drawImpl(bool*) override;

private:
  stk::StkFloat  _value;
};

#endif // TX_VALUE_H