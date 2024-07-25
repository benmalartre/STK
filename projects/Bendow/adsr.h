#include <Stk.h>
#include <ADSR.h>
#include "node.h"

#ifndef TX_ADSR_H
#define TX_ADSR_H


class TxAdsr : public TxNode {
public:
  enum Parameters {
    ATTACK = TxNode::LAST,
    DECAY,
    SUSTAIN, 
    RELEASE,
    TRIGGER,
    LAST
  };

  TxAdsr(TxNode* parent, const std::string& name);
  ~TxAdsr();
  stk::StkFloat tick(unsigned int) override;
  void setAttack(stk::StkFloat attack);
  void setDecay(stk::StkFloat decay);
  void setSustain(stk::StkFloat sustain);
  void setRelease(stk::StkFloat release);
  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(TxEditor*, bool*) override;

private:
  stk::ADSR     _adsr;
  stk::StkFloat _attack;
  stk::StkFloat _decay;
  stk::StkFloat _sustain;
  stk::StkFloat _release;
  bool          _trigger;
};

#endif // TX_ADSR_H