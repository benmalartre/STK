#include "node.h"

#ifndef TX_TRACK_H
#define TX_TRACK_H

class TxGraph;
class TxSequencer;
class TxTrack : public TxNode {
public:
  enum Parameters {
    VOLUME = TxNode::LAST,
    TRIGGER,
    VALUE
  };
  TxTrack();
  ~TxTrack();
  const ImVec2& size() override{return ImVec2();};
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
    
  void basicGraph();
  void reset() override;

  bool active(){return _active;};
  TxGraph* graph(){return _graph;};
  Beat& beat(size_t beatIdx){
    return _sequence[beatIdx%_sequence.size()];
};
  size_t length(){return _sequence.size();};

  void setLength(size_t length);
  void setBeat(size_t length, const Beat& beat);

protected:
  void _drawImpl(TxEditor*, bool*) override;

private:
  TxGraph*  _graph;
  Sequence _sequence;
  float    _volume;
  bool     _trigger;
  float    _value;
};

#endif // TX_VALUE_H