#include <Stk.h>
#include <Filter.h>
#include "node.h"

#ifndef TX_FILTER_H
#define TX_FILTER_H


class TxFilter : public TxNode {
public:
  static const int NumFilter = 13;
  static const char* FilterName[NumFilter];

  enum Type {
    BIQUAD,
    DELAY,
    DELAYA,
    DELAYL,
    FIR,
    FORMSWEP,
    LIR,
    ONEPOLE,
    ONEZERO,
    POLEZERO,
    TWOPOLE,
    TWOZERO,
    TAPDELAY
  };

  enum Parameters {
    GAIN = TxNode::LAST,
    FLOAT1,
    FLOAT2,
    FLOAT3,
    LAST
  };
  
  TxFilter(const std::string& name);
  ~TxFilter();
  void setFilter(int filterIdx);
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;

  void draw() override;
  void reset() override;

private:
  stk::Filter*  _filter;
  int           _lastFilterIdx;
  int           _filterIdx;
  stk::StkFloat _gain;
  stk::StkFloat _pole;

};

#endif // TX_GENERATOR_H