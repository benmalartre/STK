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
    INPUT = TxNode::LAST,
    MODDEPTH,
    MODFREQUENCY,
    DELAY,
    MAXIMUMDELAY,
    LAST
  };
  
  TxEffect(TxNode* parent, const std::string& name);
  ~TxEffect();
  stk::StkFloat tick(unsigned int) override;

  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(TxEditor* editor, bool*) override;

private:
  static const ImVec2 Size;
  stk::Effect*  _effect;
  short         _effectIdx;
  stk::StkFloat _modDepth;
  stk::StkFloat _modFrequency;
  int           _delay;
  int           _maximumDelay;
};

#endif // TX_GENERATOR_H