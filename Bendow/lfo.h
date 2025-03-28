#include <Stk.h>
#include <SineWave.h>
#include "node.h"

#ifndef TX_LFO_H
#define TX_LFO_H


class TxLfo : public TxNode {
public:
  enum Parameters {
    FREQUENCY = TxNode::LAST,
    AMPLITUDE,
    OFFSET, 
    LAST
  };
  TxLfo(TxNode* parent, const std::string& name);
  ~TxLfo();
  stk::StkFloat tick(unsigned int) override;

  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(TxEditor* editor, bool*) override;

private:
  stk::SineWave   _sine;
  stk::StkFloat   _frequency;
  stk::StkFloat   _amplitude;
  stk::StkFloat   _offset;
};


#endif // TX_LFO_H