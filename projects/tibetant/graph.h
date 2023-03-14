#include <vector>
#include "common.h"
#include "node.h"

#ifndef TX_GRAPH_H
#define TX_GRAPH_H

class TxGraph {
  public:

    TxGraph(const std::string& name) 
      : _name(name)
      , _current(NULL)
      , _active(true)
    {};

    ~TxGraph()
    {
      for(auto& node: _nodes)delete node;
    };

    void setActive(bool state);
    void setCurrent(TxNode* node);
    void addNode(TxNode* node);
    void removeNode(TxNode* node);

    stk::StkFloat          tick();
    TxNode*                getCurrent();
    std::vector<TxNode*>&  getNodes();
    void                   draw();

  private:
    bool                   contains(const TxNode* node);
    int                    index(const TxNode* node);
    std::string          _name;
    std::vector<TxNode*> _nodes;
    TxNode*              _current;
    bool                 _active;
};

#endif // TX_GRAPH_H