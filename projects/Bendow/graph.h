#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_GRAPH_H
#define TX_GRAPH_H

class TxGraph {
public:
  enum Pick {
    NONE,
    HEAD,
    BODY
  };

  enum Layer {
    BACKGROUND,
    FOREGROUND,
    CNT
  };

  TxGraph(const std::string& name);
  ~TxGraph();

  void                    setActive(bool state);
  void                    setCurrent(TxNode* node);
  void                    initSpliter();
  void                    setSplitterChannel(int channel);
  void                    terminateSplitter();
  void                    addNode(TxNode* node);
  void                    removeNode(TxNode* node);
  TxConnexion*            startConnexion(TxParameter* param, int channel);
  void                    updateConnexion(TxParameter* param, int channel, bool state);
  void                    terminateConnexion();
  void                    addConnexion(TxConnexion* connexion);
  void                    removeConnexion(TxConnexion* connexion);

  const ImVec2&           offset();
  const float&            scale();
  stk::StkFloat           tick();
  TxNode*                 current();
  TxConnexion*            connexion();
  std::vector<TxNode*>&   nodes();
  void                    draw();

protected:            
  static const int        Flags;
  TxNode*                 pick(const ImVec2& position);
  void                    select(const ImVec2& position);
  bool                    contains(const TxNode* node);
  int                     index(const TxNode* node);
  void                    handleInput();

private:
  void                      _createNodeByType(int type);
  void                      _drawNode(TxNode* node, bool* modified);
  void                      _drawConnexion(TxConnexion* connexion);
  void                      _drawPopup();
  void                      _drawGrid();
  std::string               _name;
  std::vector<TxNode*>      _nodes;
  std::vector<TxConnexion*> _connexions;
  TxNode*                   _current;
  TxConnexion*              _connexion;
  bool                      _active;

  ImDrawListSplitter        _splitter;
  TxNode*                   _hovered;
  TxNode*                   _pick;
  bool                      _drag;
  ImVec2                    _offset;
  float                     _scale;
};

#endif // TX_GRAPH_H