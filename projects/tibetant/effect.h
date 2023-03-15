#include <Stk.h>
#include <Effect.h>
#include "node.h"

#ifndef TX_EFFECT_H
#define TX_EFFECT_H


class TxEffect : public TxNode {
public:
  enum Type {
    CHORUS,
    ECHO,
    FREEVERB,
    JCREV,
    LENPITSHIFT,
    NREV,
    PITSHIFT,
    PRCREV
  };

  enum Parameters {
    MODDEPTH = TxNode::LAST,
    MODFREQUENCY,
    DELAY,
    MAXIMUMDELAY,
    LAST
  };
  
  TxEffect(const std::string& name);
  ~TxEffect();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;

  void draw() override;
  void reset() override;

private:
  stk::Effect*  _effect;
  short         _effectIdx;
  stk::StkFloat _modDepth;
  stk::StkFloat _modFrequency;
  int           _delay;
  int           _maximumDelay;
};

#endif // TX_GENERATOR_H