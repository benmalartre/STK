#include <bitset>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include <Instrmnt.h>
#include "generator.h"

#ifndef TX_SEQUENCER_H
#define TX_SEQUENCER_H

class Sequencer {
public:
  using Time = std::bitset<8>;
  using Sequence = std::vector<Time>;

  class Track {
    public:

    Track(uint64_t length=64) : _times(Sequence(length)){};

    void setLength(uint64_t length);
    void setWaveForm(int index);
    void setTime(uint64_t time, const Time& value);

    private:
    Sequence        _times;
    WaveGenerator   _generator;
  };

private:
  std::vector<Track>      _tracks;
  uint32_t                _n;
  uint32_t                _bpm;
  double                  _time;
  uint64_t                _length;
  bool                    _running;

public:
  Sequencer();
  Sequencer(uint32_t bpm, uint32_t n, uint64_t length);
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  uint32_t numTracks();
  Track* addTrack();
  void removeTrack(uint32_t trackIdx);
  void setTime(uint32_t trackIdx, uint64_t time, const Time& value);
  int get(uint32_t trackIdx, uint64_t timeIdx);
  void start();
  void stop();
  float tick(size_t trackIdx);
  uint64_t timeToIndex(double time);

  
};

#endif // TX_SEQUENCER_H