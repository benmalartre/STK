#include <Stk.h>
#include <vector>
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H

class TxNode {
enum Parameters {
  ACTIVE = 0,
  VOLUME = 1,
  STEREO = 2
};

public:
  TxNode(const std::string& name, uint32_t numChannels=1);
  ~TxNode();
  virtual stk::StkFloat tick(void) = 0;
  virtual stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  void setActive(bool state);
  void setDirty(bool state);
  void setStereo(const stk::StkFloat& stereo);
  stk::StkFloat stereoWeight(uint32_t channelIdx);
  bool connect(TxNode* node, const std::string& parameter);
  virtual void draw() = 0;

protected:
  std::string               _name;
  int                       _nChannels;
  bool                      _active;
  bool                      _dirty;
  stk::StkFloat             _volume;
  stk::StkFloat             _stereo;
  stk::StkFloat             _sample[2];
  std::vector<TxParameter*> _inputs;
};

#endif // TX_NODE_H