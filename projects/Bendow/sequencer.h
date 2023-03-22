#include <bitset>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "node.h"

#ifndef TX_SEQUENCER_H
#define TX_SEQUENCER_H

class TxSequencer : public TxNode {
public:
  using Beat = std::pair<short, float>;
  using Sequence = std::vector<Beat>;
  using Index = std::pair<uint32_t, uint32_t>;

  static const uint32_t NumBits = 4;

  enum Parameters {
    TRIGGER = TxNode::LAST,
    BPM,
    LAST
  };

public:
  TxSequencer(TxGraph* parent, const std::string& name);
  TxSequencer(TxGraph* parent, const std::string& name, uint32_t bpm, uint64_t length);
  ~TxSequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  
  void setBeat(uint64_t time, const Beat& value);
  stk::StkFloat tick(unsigned int channel) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  
  const ImVec2& size() override;
  void start();
  void stop();
  Index timeToIndex(float time);

  bool drawBeat(uint32_t beatIdx, uint32_t bitIdx, bool active);
  void reset() override;

protected: 
  void _drawImpl(bool*) override;
private:
  static ImVec2       Size;
  Sequence            _sequence;
  int                 _bpm;
  int                 _length;
  bool                _running;

  
};

#endif // TX_SEQUENCER_H