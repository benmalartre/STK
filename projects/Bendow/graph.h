#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_GRAPH_H
#define TX_GRAPH_H

class TxGraph {
  public:
    TxGraph(const std::string& name);
    ~TxGraph();

    void setActive(bool state);
    void setCurrent(TxNode* node);
    void addNode(TxNode* node);
    void removeNode(TxNode* node);

    stk::StkFloat          tick();
    TxNode*                getCurrent();
    std::vector<TxNode*>&  getNodes();
    void                   draw();

  private:
    void                   pick(const ImVec2& position);
    bool                   contains(const TxNode* node);
    int                    index(const TxNode* node);
    std::string          _name;
    std::vector<TxNode*> _nodes;
    TxNode*              _current;
    bool                 _active;

    ImVec2               _offset;
    ImVec2               _scale;
};

#endif // TX_GRAPH_H