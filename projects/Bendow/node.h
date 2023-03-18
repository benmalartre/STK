#include <Stk.h>
#include <vector>
#include "buffer.h"
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
  enum State {
    ON,
    OFF
  };

  enum Parameters {
    SAMPLES,
    LAST
  };

  TxNode(const std::string& name, uint32_t numChannels=1);
  virtual ~TxNode();
  virtual stk::StkFloat tick(unsigned int=0) = 0;
  virtual stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  const std::string& name();
  void setDirty(bool state);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  TxParameter* getParameter(const std::string& name);
  void commonControls();
  virtual void draw() = 0;
  virtual void reset() = 0;

protected:
  std::string               _name;
  int                       _nChannels;
  int                       _state;
  bool                      _dirty;
  stk::StkFrames            _frames;
  TxBuffer                  _buffer;
  std::vector<TxParameter*> _params;
};


#endif // TX_NODE_H