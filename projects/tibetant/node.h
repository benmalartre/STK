#include <Stk.h>
#include <vector>
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H

class TxNode {
public:
  enum Parameters {
    SAMPLES,
    ACTIVE,
    VOLUME,
    STEREO,
    LAST
  };

  TxNode(const std::string& name, uint32_t numChannels=1);
  ~TxNode();
  virtual stk::StkFloat tick(void) = 0;
  virtual stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  void setActive(bool state);
  void setDirty(bool state);
  void setStereo(stk::StkFloat stereo);
  void setVolume(stk::StkFloat volume);
  stk::StkFloat stereoWeight(uint32_t channelIdx);
  bool connect(TxNode* node, const std::string& name);
  void disconnect(const std::string& name);
  TxParameter* getParameter(const std::string& name);
  void draw();

  
protected:
  std::string               _name;
  int                       _nChannels;
  bool                      _active;
  bool                      _dirty;
  stk::StkFloat             _volume;
  stk::StkFloat             _stereo;
  stk::StkFrames            _frames;
  std::vector<TxParameter*> _params;
};


#endif // TX_NODE_H