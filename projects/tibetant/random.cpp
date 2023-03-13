#include "common.h"
#include "random.h"

TxRandom::TxRandom(const std::string& name)
  : TxNode(name)
  , _minimum(-1.f)
  , _maximum(1.f)
  , _seed(0)
  , _cnt(INT_MAX)
  , _frequency(1.f)
{
  _rate = stk::Stk::sampleRate() / _frequency;
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

void TxRandom::setFrequency(stk::StkFloat frequency)
{
  _frequency = frequency;
  _rate = stk::Stk::sampleRate()  / _frequency;
}

stk::StkFloat TxRandom::tick(void)
{
  if(_cnt++ > _rate) {
    _cnt = 0;
    _frames[0] = RANDOM_LO_HI(_minimum, _maximum);;
  }
  return _frames[0];
}

stk::StkFrames& TxRandom::tick(stk::StkFrames& frames, unsigned int channel)
{
  for(size_t i = 0; i < frames.size(); ++i) {
     if(_cnt++ > _rate) {
        _cnt = 0;
        _frames[0] = RANDOM_LO_HI(_minimum, _maximum);;
    }
    frames[i] = _frames[0];
  }
  return frames;
}

void TxRandom::draw()
{
  ImGui::Begin(_name.c_str(), NULL);

  commonControls();

  ImGui::End();
}