#include "node.h"

#ifndef TX_ARYTHMETIC_H
#define TX_ARYTHMETIC_H

class TxArythmetic : public TxNode {
public:
  static const int NumMode = 5;
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
    FLOAT1,
    LAST
  };

  TxArythmetic(TxNode* parent, const std::string& name);
  ~TxArythmetic();
  stk::StkFloat tick(unsigned int) override;
  void setMode(int mode);
  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(TxEditor* editor, bool* modified) override;

private:
  static ImVec2  Size;
  int     _mode;
  float   _float1;
};

#endif // TX_ARYTHMETIC_H