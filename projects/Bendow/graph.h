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
  TxGraph(const std::string& name);
  ~TxGraph();

  void                    setActive(bool state);
  void                    setCurrent(TxNode* node);
  void                    addNode(TxNode* node);
  void                    removeNode(TxNode* node);
  void                    addConnexion(TxConnexion* connexion);
  void                    removeConnexion(TxConnexion* connexion);

  const ImVec2&           offset();
  const float&            scale();
  stk::StkFloat           tick();
  TxNode*                 current();
  std::vector<TxNode*>&   nodes();
  void                    draw();

protected:            
  static const int        Flags;
  int                     pick(const ImVec2& position);
  void                    select(const ImVec2& position);
  bool                    contains(const TxNode* node);
  int                     index(const TxNode* node);

private:
  void                      _drawPopup();
  void                      _drawGrid();
  std::string               _name;
  std::vector<TxNode*>      _nodes;
  std::vector<TxConnexion*> _connexions;
  std::vector<bool>         _selection;
  TxNode*                   _hovered;
  TxNode*                   _current;
  bool                      _active;

  int                       _pick;
  bool                      _drag;
  ImVec2                    _offset;
  float                     _scale;
};

#endif // TX_GRAPH_H