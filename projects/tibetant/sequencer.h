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
  using Time = std::pair<bool, float>;
  using Sequence = std::vector<Time>;

public:
  TxSequencer();
  TxSequencer(uint32_t bpm, uint64_t length);
  ~TxSequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  
  void setTime(uint64_t time, const Time& value);
  stk::StkFloat tick(unsigned int channel) override;
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  
  void start();
  void stop();
  uint64_t timeToIndex(float time);

  void drawTime(uint64_t timeIdx, bool active);
  void draw() override;

private:
  Sequence            _sequence;
  int                 _bpm;
  float               _time;
  int                 _length;
  bool                _running;

  
};

#endif // TX_SEQUENCER_H