#include "common.h"

Index TxTime::index(size_t length)
{
  const float relativeTime = _time / 60.f * (float)_bpm * NumBits;
  const int beatTime = (int)relativeTime;
  const int bitTime = (int)((float)(relativeTime - beatTime) * NumBits);
  return {beatTime % length, bitTime};
}
