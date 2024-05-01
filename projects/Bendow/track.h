#include "node.h"

#ifndef TX_TRACK_H
#define TX_TRACK_H

class TxGraph;
class TxSequencer;
class TxTrack : public TxNode {
public:
  enum Parameters {
    VOLUME = TxNode::LAST,
    MINIMUM,
    MAXIMUM, 
    SEED
  };
  TxTrack();
  ~TxTrack();
  const ImVec2& size() override{return ImVec2();};
  stk::StkFloat tick(unsigned int) override;
  stk::StkFrames& tick(stk::StkFrames&, unsigned int channel) override;
    
  void setBasicGraph();
  void reset() override;

  bool active(){return _active;};
  TxGraph* graph(){return _graph;};
  Beat& beat(size_t beatIdx){return _sequence[beatIdx];};
  size_t length(){return _sequence.size();};

  void setLength(size_t length);
  void setBeat(size_t length, const Beat& beat);

protected:
  void _drawImpl(TxEditor*, bool*) override;

private:
  TxGraph*  _graph;
  Sequence _sequence;
};

#endif // TX_VALUE_H