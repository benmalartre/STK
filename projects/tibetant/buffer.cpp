#include "buffer.h"

TxBuffer::TxBuffer() 
  : _n((size_t)stk::RT_BUFFER_SIZE * 4)
  , _smin(-1.f)
  , _smax(1.f)
{
  _samples.resize(_n * 2);
  _p = _n;
}

TxBuffer::TxBuffer(size_t size, float smin, float smax)
  : _n(size)
  , _smin(smin)
  , _smax(smax)
{
  _samples.resize(_n * 2);
  _p = _n;
}

void TxBuffer::resize(size_t size)
{
  if(size == _samples.size())return;
  _samples.resize(size);
}

void TxBuffer::write(float sample)
{
  _samples[_p++] = sample;
  if(_p >= _samples.size()) {
    swap();
  }
}

const float* TxBuffer::read()
{
  return &_samples[_p - _n];
}

void TxBuffer::scale(float smin, float smax)
{
  _smin = smin;
  _smax = smax;
}

void TxBuffer::swap()
{
  size_t n = _n + 1;
  stk::StkFloat *dst = &_samples[0];
  stk::StkFloat *src = &_samples[n - 1];
  while(n-- > 0) {
    *dst++ = *src++;
  }
  _p = _n;
}

void TxBuffer::draw()
{
  ImGui::PlotConfig conf;
  //conf.values.xs = x_data; // this line is optional
  conf.values.ys = read();
  conf.values.count = _n;
  conf.scale.min = _smin;
  conf.scale.max = _smax;
  conf.tooltip.show = true;
  conf.tooltip.format = "x=%.2f, y=%.2f";
  conf.grid_x.show = false;
  conf.grid_y.show = false;
  conf.grid_x.size = 1;
  conf.grid_y.size = 1;
  conf.frame_size = ImVec2(256, 128);
  conf.line_thickness = 0.1f;

  ImGui::Plot("Buffer", conf);
}