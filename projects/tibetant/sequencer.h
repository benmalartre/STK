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

class Sequencer : public TxNode {
public:
  using Time = uint8_t;
  using Sequence = std::vector<Time>;

  class Track {
  public:

    Track(const std::string& name, uint64_t length=64) 
      : _times(Sequence(length))
      , _node(NULL)
    {};

    ~Track() {};

    void setLength(uint64_t length);
    void setTime(uint64_t time, const Time& value);
    void setNode(TxNode* node);

    stk::StkFloat tick(uint64_t timeIdx);
    void draw();

  private:
    Sequence        _times;
    TxNode*         _node;
  };

private:
  std::vector<Track*> _tracks;
  int                 _n;
  int                 _bpm;
  double              _time;
  int                 _length;
  bool                _running;

public:
  Sequencer();
  Sequencer(uint32_t bpm, uint32_t n, uint64_t length);
  ~Sequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  uint32_t numTracks();
  Track* addTrack(const std::string& name);
  void removeTrack(uint32_t trackIdx);
  std::vector<Track*>& getTracks(){return _tracks;};
  Track* getTrack(uint32_t trackIdx);
  void setTime(uint32_t trackIdx, uint64_t time, const Time& value);
  stk::StkFloat tick(void) override;
  stk::StkFloat tick(uint32_t trackIdx);
  stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) override;
  
  
  void start();
  void stop();
  uint64_t timeToIndex(double time);
  void draw() override;

  
};

#endif // TX_SEQUENCER_H