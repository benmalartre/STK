#include "common.h"
#include "random.h"

TxRandom::TxRandom(const std::string& name)
  : TxNode(name)
  , _minimum(-1.f)
  , _maximum(1.f)
  , _seed(0)
{
  srand(_seed);
}

TxRandom::~TxRandom()
{

}

void TxRandom::setMinimum(stk::StkFloat value)
{
  _minimum = value;
}

void TxRandom::setMaximum(stk::StkFloat value)
{
  _maximum = value;
}

void TxRandom::setSeed(int seed)
{
  _seed = seed;
  srand(_seed);
}

stk::StkFloat TxRandom::tick(void)
{
  return RANDOM_LO_HI(_minimum, _maximum);
}

stk::StkFrames& TxRandom::tick(stk::StkFrames& frames, unsigned int channel)
{
  for(size_t i = 0; i < frames.size(); ++i) {
    frames[i] = RANDOM_LO_HI(_minimum, _maximum);
  }
  return frames;
}