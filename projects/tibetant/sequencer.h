#include <bitset>
#include <vector>
#include <chrono>
#include <thread>
#include <time.h>
#include <iostream>

class Sequencer {
public:
  using Time = std::bitset<8>;
  using Track = std::vector<Time>;

private:
  std::vector<Track>   _tracks;
  uint32_t             _n;
  uint32_t             _bpm;
  double               _time;
  uint64_t             _length;
  bool                 _running;

public:
  Sequencer();
  Sequencer(uint32_t bpm, uint32_t n, uint64_t length);
  void SetLength(uint64_t length);
  void SetBPM(uint32_t bpm);
  uint32_t NumTracks();
  Track* AddTrack();
  void RemoveTrack(uint32_t trackIdx);
  void SetTime(uint32_t trackIdx, uint64_t time, const Time& value);
  int Get(uint32_t trackIdx, uint64_t timeIdx);
  void Start();
  void Stop();
  void Tick();
  uint64_t TimeToIndex(double time);

  
};