#include "node.h"

#ifndef TX_VALUE_H
#define TX_VALUE_H

class TxValue : public TxNode {
public:
  enum Parameters {
    VALUE = TxNode::LAST
  };
  TxValue(TxNode* parent, const std::string& name);
  ~TxValue();
  stk::StkFloat tick(unsigned int) override;
  void setValue(const stk::StkFloat& value);
  void reset() override;

  const ImVec2& size() override;

protected:

  void _drawImpl(TxEditor*, bool*) override;

private:
  static ImVec2  Size;
  stk::StkFloat  _value;
};

#endif // TX_VALUE_H