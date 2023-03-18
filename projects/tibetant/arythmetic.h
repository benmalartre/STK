#include "node.h"

#ifndef TX_ARYTHMETIC_H
#define TX_ARYTHMETIC_H

class TxArythmetic : public TxNode {
public:
  static const int NumMode = 4;
  static const char* ModeName[NumMode];

  enum Mode {
    ADD,
    SUB,
    MUL,
    DIV,
    MIX
  };

  enum Parameters {
    MODE = TxNode::LAST,
    INPUT1,
    INPUT2, 
    LAST
  };

  TxArythmetic(const std::string& name);
  ~TxArythmetic();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setMode(int mode);
  void draw() override;
  void reset() override;

private:
  int _mode;
};

#endif // TX_ARYTHMETIC_H