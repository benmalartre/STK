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
    INPUT = TxNode::LAST,
    FILTER,
    GAIN,
    FLOAT1,
    FLOAT2,
    FLOAT3,
    INT1,
    INT2,
    INT3,
    BOOL1,
    LAST
  };
  
  TxFilter(TxGraph* parent, const std::string& name);
  ~TxFilter();
  void setFilter(int filterIdx);
  stk::StkFloat tick(unsigned int) override;

  const ImVec2& size() override;
  void reset() override;

protected:
  void _drawImpl(bool*) override;

private:
  static ImVec2 Size;
  stk::Filter*  _filter;
  int           _lastFilterIdx;
  int           _filterIdx;
  stk::StkFloat _gain;
  stk::StkFloat _float1;
  stk::StkFloat _float2;
  stk::StkFloat _float3;
  int           _int1;
  int           _int2;
  int           _int3;
  bool          _bool1;

};

#endif // TX_GENERATOR_H