#include "common.h"
#include "effect.h"


TxEffect::TxEffect(const std::string& name) 
  : TxNode(name)
{
}

TxEffect::~TxEffect() 
{
}

stk::StkFloat TxEffect::tick(void)
{
  return 0.f;// _effect->tick();
}

stk::StkFloat TxEffect::tick(stk::StkFloat input)
{
  return 0.f;
}


stk::StkFrames& TxEffect::tick(stk::StkFrames& frames, unsigned int channel)
{
  return frames;
}