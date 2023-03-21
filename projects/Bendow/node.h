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
  static const int Flags;
  static const int PortSize;

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
  const ImVec2& position();
  virtual const ImVec2& size() = 0;
  const ImVec4& color();
  void setDirty(bool state);
  void setPosition(const ImVec2& pos);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  TxParameter* getParameter(const std::string& name);
  void commonControls();
  void drawInput();
  int pick(const ImVec2& pos);
  
  void draw(const ImVec2& offset, const float& scale, bool* modified);
  virtual void reset() = 0;

protected:
  virtual void              _drawImpl(bool* modified) {};
  ImVec2                    _position;
  ImVec2                    _size;
  ImVec4                    _color;
  int                       _expended;

  std::string               _name;
  int                       _nChannels;
  bool                      _dirty;
  stk::StkFrames            _frames;
  TxBuffer                  _buffer;
  std::vector<TxParameter*> _params;
};


#endif // TX_NODE_H