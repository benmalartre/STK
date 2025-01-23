#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

#include "common.h"
#include "node.h"
#include "buffer.h"
#include "graph.h"
#include "track.h"
#include "record.h"

#ifndef TX_SEQUENCER_H
#define TX_SEQUENCER_H


class TxSequencer : public TxNode {
public:
  enum Parameters {
    TRIGGER = TxNode::LAST,
    RUNNING,
    BPM,
    VOLUME,
    TIME,
    LAST
  };

  static size_t TRACK_INDEX;

public:
  TxSequencer(uint32_t numTracks);
  TxSequencer(uint32_t numTracks, uint32_t bpm, uint64_t length);
  ~TxSequencer();
  void setLength(uint64_t length);
  void setBPM(uint32_t bpm);
  void toggleRunning();
  
  void setBeat(uint32_t trackIdx, uint64_t time, const Beat& value);
  stk::StkFloat tick(unsigned int channel) override;

  Beat* beat(uint32_t trackIdx, size_t beatIdx);
  
  const ImVec2& size() override;
  int bpm(){return _bpm;};
  bool running(){return _running;};
  void start();
  void stop();

  void reset() override;
  void draw(TxEditor* editor);

  TxTrack* track(size_t index);
  std::vector<TxTrack*>& tracks(){return _tracks;};
  size_t numTracks(){return _tracks.size();};
  void addTrack();
  void removeTrack(TxTrack* track);
  
protected:
  static const int        Flags;

private:
  void _drawImpl(TxEditor* editor, bool* modified) override{};

  TxRecorder            _recorder;

  std::vector<TxTrack*> _tracks;
  TxTrack*              _current;
  int                   _bpm;
  int                   _length;
  bool                  _running;
  float                 _time;
  float                 _volume;
};

#endif // TX_SEQUENCER_H