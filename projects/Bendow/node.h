#include <Stk.h>
#include <vector>
#include "buffer.h"
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H


class TxNode;
class TxGraph;

#define TX_TITLE_X 64.f
#define TX_TITLE_Y 8.f
#define TX_PADDING_X 16.f
#define TX_PADDING_Y 4.f
#define TX_PLUG_WIDTH 24.f
#define TX_PLUG_HEIGHT 18.f
#define TX_PLUG_DETAIL 6.f

struct TxConnexion {
  TxParameter* source;
  TxParameter* target;
  int channel;
};

class TxNode {

public:
  static const int Flags;
  static const int PortSize;

  static const int NumNode = 10;
  static const char* NodeName[NumNode];

  enum Type {
    VALUE,
    RANDOM,
    OSCILLATOR,
    SEQUENCER,
    LFO,
    ADSR,
    ARYTHMETIC,
    FILTER,
    EFFECT,
    MIXER
  };

  enum Parameters {
    OUTPUT,
    LAST
  };

  TxNode(TxGraph* parent, const std::string& name, uint32_t numChannels=1);
  virtual ~TxNode();
  virtual stk::StkFloat tick(unsigned int=0) = 0;
  virtual stk::StkFrames& tick(stk::StkFrames& frames, unsigned int channel) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  TxGraph* graph();
  const std::string& name();
  const ImVec2& position();
  virtual const ImVec2& size() = 0;
  const ImVec4& color();
  void setDirty(bool state);
  void setPosition(const ImVec2& pos);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  TxParameter* parameter(const std::string& name);
  int pick(const ImVec2& pos);
  
  void draw(bool selected, bool* modified);
  virtual void reset() = 0;

protected:
  void                      _drawOutput();
  void                      _drawAlignLeft();
  virtual void              _drawImpl(bool* modified) {};
  TxGraph*                  _parent;
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