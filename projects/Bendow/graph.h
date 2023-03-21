#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_GRAPH_H
#define TX_GRAPH_H

class TxGraph {
  public:
    enum Pick {
      NONE,
      NODE,
      INPUT,
      OUTPUT
   };
    TxGraph(const std::string& name);
    ~TxGraph();

    void setActive(bool state);
    void setCurrent(TxNode* node);
    void addNode(TxNode* node);
    void removeNode(TxNode* node);

    stk::StkFloat          tick();
    TxNode*                current();
    TxNode*                selected();
    std::vector<TxNode*>&  nodes();
    void                   draw();

  private:
    static const int       Flags;
    int                    pick(const ImVec2& position);
    bool                   contains(const TxNode* node);
    int                    index(const TxNode* node);
    std::string          _name;
    std::vector<TxNode*> _nodes;
    TxNode*              _current;
    TxNode*              _selected;
    bool                 _active;

    bool                 _drag;
    ImVec2               _offset;
    float                _scale;
};

#endif // TX_GRAPH_H