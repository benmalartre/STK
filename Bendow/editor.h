#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_EDITOR_H
#define TX_EDITOR_H


static size_t SplitterHeight = 250;
extern TxTrack* currentTrack;

class TxSequencer;
class TxEditor {
public:
  enum Layer {
    BACKGROUND,
    FOREGROUND,
    CNT
  };

  TxEditor(TxGraph* graph, bool navigatable=false);
  virtual ~TxEditor(){};
  void                    initSpliter();
  void                    setSplitterChannel(int channel);
  void                    terminateSplitter();

  void                    startConnexion(TxParameter* param, int channel);
  void                    updateConnexion(TxParameter* param, int channel);
  void                    terminateConnexion();

  const ImVec2&           pos();
  const ImVec2&           offset();
  const float&            scale();
  TxNode*                 current();
  TxConnexion*            connexion();

  virtual void            resize(size_t splitterHeight) = 0;
  virtual void            draw() = 0;

  TxNode*                 pick(const ImVec2& p);
  void                    select(const ImVec2& p);
  void                    handleInput();

  void                    setCurrent(TxTrack* track);
  void                    setGraph(TxGraph* graph);


protected:            
  static const int        Flags;

  ImVec2                    _pos;
  ImVec2                    _size;
  ImDrawListSplitter        _splitter;
  bool                      _drag;
  bool                      _pan;
  ImVec2                    _offset;
  float                     _scale;
  float                     _invScale;
  bool                      _navigatable;

  TxGraph*                  _graph;

  TxTrack*                  _current;
  TxConnexion*              _connexion;

  TxNode*                   _hovered;
  TxNode*                   _pick;

};

class TxGraphEditor : public TxEditor{
public:
  enum Pick {
    NONE,
    HEAD,
    BODY
  };

  TxGraphEditor(TxGraph* graph);
  ~TxGraphEditor();

  void                    resize(size_t splitterHeight) override;
  void                    draw() override;

  bool                    contains(const TxNode* node);

protected:
  void                      _createNodeByType(int type);
  void                      _drawNode(TxNode* node, bool* modified);
  void                      _drawConnexion(TxConnexion* connexion);
  void                      _drawPopup();
  void                      _drawGrid();
  void                      _drawFrame();
};

class TxSequencerEditor : public TxEditor {
public:
  TxSequencerEditor(TxSequencer* sequencer);
  ~TxSequencerEditor();

  void            resize(size_t splitterHeight) override;
  void            draw() override;

protected:
  bool _drawBeat(TxTrack* track, bool expended, uint32_t beatIdx, uint32_t bitIdx, bool current);

private:
  TxSequencer*    _sequencer;
};


#endif // TX_EDITOR_H