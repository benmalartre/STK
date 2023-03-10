#include <bitset>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "generator.h"

#ifndef TX_SEQUENCER_H
#define TX_SEQUENCER_H

class Sequencer {
public:
  using Time = uint8_t;
  using Sequence = std::vector<Time>;

  class Track {
  public:

    Track(uint64_t length=64) 
      : _times(Sequence(length))
      , _generator(new TxGenerator())
    {};

    ~Track() {delete _generator;};

    void setLength(uint64_t length);
    void setWaveForm(int index);
    void setTime(uint64_t time, const Time& value);
    void noteOn(size_t timeIdx);
    void noteOff();

    stk::StkFrames& tick(uint64_t timeIdx);
    const stk::StkFrames& frames() const;
    const stk::StkFloat* samples() const;
    void draw();

  private:
    Sequence        _times;
    TxGenerator*    _generator;
    float           _stereo; // -1.0 (full left) -> 1.0 (full right), default 0.0 (balanced)
  };

private:
  std::vector<Track*>     _tracks;
  uint32_t                _n;
  uint32_t                _bpm;
  double                  _time;
  uint64_t                _length;
  bool                    _running;

public:
  Sequencer();
  Sequencer(uint32_t bpm, uint32_t n, uint64_t length);
  ~Sequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  uint32_t numTracks();
  Track* addTrack();
  void removeTrack(uint32_t trackIdx);
  std::vector<Track*>& getTracks(){return _tracks;};
  Track* getTrack(uint32_t trackIdx);
  void setTime(uint32_t trackIdx, uint64_t time, const Time& value);
  stk::StkFrames& tick(stk::StkFrames& frames);
  
  void start();
  void stop();
  uint64_t timeToIndex(double time);

  
};

#endif // TX_SEQUENCER_H