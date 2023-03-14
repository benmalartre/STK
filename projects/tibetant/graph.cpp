#include "graph.h"

bool TxGraph::contains(const TxNode* node)
{
  for(auto& n: _nodes) {
    if(node == n)return true;
  }
  return false;
}

int TxGraph::index(const TxNode* node)
{
  for(size_t i = 0; i < _nodes.size(); ++i) {
    if(_nodes[i] == node)return i;
  }
  return -1;
}

void TxGraph::setActive(bool state)
{
  _active = state;
}

void TxGraph::setCurrent(TxNode* node)
{
  _current = node;
}

TxNode* TxGraph::getCurrent()
{
  return _current;
}

std::vector<TxNode*>& TxGraph::getNodes()
{
    return _nodes;
}

void TxGraph::addNode(TxNode* node) 
{
  if(!contains(node)) {
    _nodes.push_back(node);
    _current = node;
  }
}

void TxGraph::removeNode(TxNode* node) 
{
  int idx = index(node);
  if(idx >= 0) {
    _nodes.erase(_nodes.begin() + idx);
    delete node;
  }
}

stk::StkFloat TxGraph::tick()
{
  if(_current) return _current->tick();
  return 0.f;
}

void TxGraph::draw()
{
  for(auto& node: _nodes)node->draw();
}