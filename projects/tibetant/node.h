#include <Stk.h>
#include <vector>
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H


class TxNode;

struct TxConnexion {
  TxNode*       start;
  TxNode*       end;
  TxParameter*  target;
};

class TxNode {
public:

  enum Parameters {
    SAMPLES,
    ACTIVE,
    VOLUME,
    LAST
  };

  TxNode(const std::string& name, uint32_t numChannels=1);
  virtual ~TxNode();
  virtual stk::StkFloat tick(unsigned int=0) = 0;
  virtual stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  void setActive(bool state);
  void setDirty(bool state);
  void setVolume(stk::StkFloat volume);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  TxParameter* getParameter(const std::string& name);
  void commonControls();
  virtual void draw() = 0;

protected:
  std::string               _name;
  int                       _nChannels;
  bool                      _active;
  bool                      _dirty;
  stk::StkFloat             _volume;
  stk::StkFrames            _frames;
  std::vector<TxParameter*> _params;
};


#endif // TX_NODE_H