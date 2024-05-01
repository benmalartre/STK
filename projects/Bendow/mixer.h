#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "node.h"
#include "track.h"

#ifndef TX_MIXER_H
#define TX_MIXER_H

class TxMixer : public TxNode {
private:
  std::vector<TxTrack*> _tracks;
  int                   _n;

public:
  TxMixer();
  TxMixer(uint32_t bpm, uint32_t n, uint64_t length);
  ~TxMixer();
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

  void draw() override;
  void reset() override;
  
};

#endif // TX_SEQUENCER_H