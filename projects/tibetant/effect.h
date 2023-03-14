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
    FREQUENCY,
    HARMONICS, 
    LAST
  };
  
  TxEffect(const std::string& name);
  ~TxEffect();
  stk::StkFloat tick(unsigned int) override;
  stk::StkFloat tick(stk::StkFloat input);
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;

private:
  stk::Effect* _effect;
};

#endif // TX_GENERATOR_H