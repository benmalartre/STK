#include <Stk.h>
#include <vector>
#include "fonts.h"
#include "buffer.h"
#include "parameter.h"

#ifndef TX_NODE_H
#define TX_NODE_H


class TxNode;
class TxGraph;
class TxTrack;


struct TxConnexion;

class TxNode {

public:
  static const int Flags;
  static const int PortSize;

  static const int NumNode = 11;
  static const int NumExposedNode = 8;
  static const char* NodeName[NumNode];

  enum Type {
    VALUE,
    RANDOM,
    OSCILLATOR,
    LFO,
    ADSR,
    ARYTHMETIC,
    FILTER,
    EFFECT,
    GRAPH,
    TRACK,
    SEQUENCER
  };

  enum Parameters {
    ACTIVE,
    OUTPUT,
    LAST
  };

  TxNode(TxNode* parent, short type, const std::string& name, uint32_t numChannels=1);
  virtual ~TxNode();
  virtual stk::StkFloat tick(unsigned int=0) = 0;
  stk::StkFloat lastSample(unsigned int channel);
  int numChannels();
  
  bool selected();
  short type();
  const std::string& name();
  const ImVec2& position();
  virtual const ImVec2& size() = 0;
  const ImU32 color();
  const ImU32 color(short colorIdx);
  TxNode* parent();
  TxConnexion* connexion(TxParameter* dst);
  TxGraph* graph();
  TxTrack* track();
  TxParameter* parameter(const std::string& name);

  void setDirty(bool state);
  void setSelected(bool state);
  void setPosition(const ImVec2& pos);
  TxConnexion* connect(TxNode* node, const std::string& name, short channel=0);
  void disconnect(const std::string& name);
  
  int pick(const ImVec2& pos);
  virtual void draw(TxEditor* editor, bool* modified);
  virtual void reset() = 0;

protected:
  void                      _drawPopup(TxEditor* editor);
  void                      _drawInput(TxEditor* editor, TxParameter* parameter, short index);
  void                      _drawOutput(TxEditor* editor);
  void                      _drawAlignLeft(TxEditor* editor);
  void                      _drawAlignTop(TxEditor* editor);
  virtual void              _drawImpl(TxEditor* editor, bool* modified) = 0;

  TxNode*                   _parent;
  ImVec2                    _position;
  ImVec2                    _size;
  ImVec4                    _color;
  short                     _type;
  int                       _expended;
  bool                      _selected;
  bool                      _active;

  std::string               _name;
  int                       _nChannels;
  bool                      _dirty;
  stk::StkFrames            _frames;
  std::vector<TxParameter*> _params;

  TxBuffer                  _buffer;
};

struct TxConnexion {
  TxParameter* source;
  TxParameter* target;
  int channel;
};


#endif // TX_NODE_H