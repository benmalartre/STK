#include <bitset>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "node.h"
#include "generator.h"

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
      , _generator(new TxGenerator(name))
    {};

    ~Track() {delete _generator;};

    void setLength(uint64_t length);
    void setWaveForm(int index);
    void setTime(uint64_t time, const Time& value);
    void setFrequency(float frequency);
    stk::StkFloat channelWeight(uint32_t channelIdx);

    stk::StkFrames& tick(uint64_t timeIdx);
    const stk::StkFrames& frames() const;
    const stk::StkFloat* samples() const;
    void draw();

  private:
    Sequence        _times;
    TxGenerator*    _generator;
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
  stk::StkFrames& tick();
  
  void start();
  void stop();
  uint64_t timeToIndex(double time);
  void draw();

  
};

#endif // TX_SEQUENCER_H