#include <Stk.h>
#include <vector>
#include "buffer.h"
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H


class TxNode;
class TxGraph;

extern ImFont* TX_FONT_BASE;
extern ImFont* TX_FONT_TITLE;

struct TxConnexion {
  TxParameter* source;
  TxParameter* target;
  int sourceChannel;
  int targetChannel;
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
  
  bool selected();
  const std::string& name();
  const ImVec2& position();
  virtual const ImVec2& size() = 0;
  const ImVec4& color();
  const ImColor& color(short colorIdx);
  TxGraph* graph();
  TxParameter* parameter(const std::string& name);

  void setDirty(bool state);
  void setSelected(bool state);
  void setPosition(const ImVec2& pos);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  
  int pick(const ImVec2& pos);
  void draw(bool* modified);
  virtual void reset() = 0;

protected:
  void                      _drawPopup();
  void                      _drawOutput();
  void                      _drawAlignLeft();
  void                      _drawAlignTop();
  virtual void              _drawImpl(bool* modified) {};
  TxGraph*                  _parent;
  ImVec2                    _position;
  ImVec2                    _size;
  ImVec4                    _color;
  int                       _expended;
  bool                      _selected;

  std::string               _name;
  int                       _nChannels;
  bool                      _dirty;
  stk::StkFrames            _frames;
  std::vector<TxParameter*> _params;
};


#endif // TX_NODE_H