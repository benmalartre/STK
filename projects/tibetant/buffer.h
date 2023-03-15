#include "common.h"
#include <vector>


#ifndef TX_BUFFER_H
#define TX_BUFFER_H

class TxBuffer {
public:
  TxBuffer();
  TxBuffer(size_t size);
  
  void resize(size_t size);
  void write(float sample);
  const float* read();
  void draw();
  void swap();

private:
 std::vector<float> _samples;
 size_t             _n;
 int                _p;
};

#endif // TX_BUFFER_H