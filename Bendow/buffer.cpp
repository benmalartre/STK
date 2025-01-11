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
  ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
  if (ImPlot::BeginPlot("##buffer", ImVec2(256,128), ImPlotFlags_CanvasOnly| ImPlotFlags_NoInputs)) {
    ImPlot::SetupAxes(0,0,ax_flags,ax_flags);
    ImPlot::SetupAxesLimits(0, _n, -1.2f, 1.2f);
    ImPlot::PlotLine("samples", read(), _n, 2.f, 2.f);
    ImPlot::EndPlot();
  }
}