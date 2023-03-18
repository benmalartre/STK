#include "common.h"
#include <vector>


#ifndef TX_BUFFER_H
#define TX_BUFFER_H

class TxBuffer {
public:
  TxBuffer();
  TxBuffer(size_t size, float smin=-1.f, float smax=1.f);
  
  void resize(size_t size);
  void write(float sample);
  const float* read();
  void draw();
  void swap();
  void scale(float smin, float smax);

private:
 std::vector<float> _samples;
 size_t             _n;
 int                _p;
 stk::StkFloat      _smin;
 stk::StkFloat      _smax;

};

#endif // TX_BUFFER_H