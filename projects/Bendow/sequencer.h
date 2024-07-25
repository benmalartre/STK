#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "node.h"
#include "graph.h"

#ifndef TX_SEQUENCER_H
#define TX_SEQUENCER_H

class TxSequencer : public TxNode {
public:
  using Beat = std::pair<short, float>;
  using Sequence = std::vector<Beat>;
  using Index = std::pair<uint32_t, uint32_t>;

  struct Track {
    bool     _active;
    TxGraph* _graph;
    Sequence _sequence;
  };

  static const uint32_t NumBits = 4;

  enum Parameters {
    TRIGGER = TxNode::LAST,
    BPM,
    TIME,
    LAST
  };

public:
  TxSequencer(uint32_t numTracks);
  TxSequencer(uint32_t numTracks, uint32_t bpm, uint64_t length);
  ~TxSequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  
  void setBeat(uint32_t trackIdx, uint64_t time, const Beat& value);
  stk::StkFloat tick(unsigned int channel) override;
  stk::StkFloat tick();
  
  const ImVec2& size() override;
  void start();
  void stop();
  Index timeToIndex(float time);

  bool drawBeat(Track* track, uint32_t beatIdx, uint32_t bitIdx, bool active, float scale);
  void reset() override;
  void draw(bool* modified) override;

  Track* getTrack(size_t index);
  
protected:
  static const int      Flags;

private:
  std::vector<Track>    _tracks;
  int                   _bpm;
  int                   _length;
  bool                  _running;
  float                 _time;
  float                 _volume;
};

#endif // TX_SEQUENCER_H