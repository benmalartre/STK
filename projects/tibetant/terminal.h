#include "node.h"
#include <vector>

#ifndef TX_TERMINAL_H
#define TX_TERMINAL_H

class TxTerminal : public TxNode {
public:
  TxTerminal(const std::string& name);
  ~TxTerminal();
  stk::StkFloat tick(void) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void draw() override;
private:
  std::vector<TxNode*> _nodes;

};

#endif // TX_VALUE_H