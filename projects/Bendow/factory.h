#include <string>
#include <map>
#include "common.h"
#ifndef TX_FACTORY_H
#define TX_FACTORY_H

class TxNode;
class TxGraph;

class TxFactory {
public:
  typedef TxNode* (*CONSTRUCTOR)(TxGraph* graph, const std::string& name);
  typedef std::pair<CONSTRUCTOR, int> Node;
  static TxFactory& get();

  static void initialize();
  static TxNode* createNodeByType(TxGraph* graph, int type);

private:
  std::map<int, Node> __NodeMap;
};


#endif // TX_FACTORY_H