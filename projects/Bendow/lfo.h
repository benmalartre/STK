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
  TxLfo(const std::string& name);
  ~TxLfo();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  void setFrequency(stk::StkFloat frequency);
  void setAmplitude(stk::StkFloat amplitude);
  void setOffset(stk::StkFloat offset);
  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(bool*) override;

private:
  static ImVec2       Size;
  stk::SineWave   _sine;
  stk::StkFloat   _frequency;
  stk::StkFloat   _amplitude;
  stk::StkFloat   _offset;
};

#endif // TX_LFO_H