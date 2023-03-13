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
  return 0.f;
}

stk::StkFloat TxEffect::tick(stk::StkFloat input)
{
  return 0.f;
}


stk::StkFrames& TxEffect::tick(stk::StkFrames& frames, unsigned int channel)
{
  return frames;
}


void TxEffect::draw()
{
  ImGui::Begin(_name.c_str(), NULL);
  

  ImGui::End();
}