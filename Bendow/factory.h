#include <string>
#include <map>
#include "common.h"
#ifndef TX_FACTORY_H
#define TX_FACTORY_H

class TxNode;
class TxGraph;

class TxFactory {
public:
  typedef TxNode* (*CONSTRUCTOR)(TxNode* parent, const std::string& name);
  typedef std::pair<CONSTRUCTOR, int> Node;
  static TxFactory& get();

  static void initialize();
  static TxNode* createNodeByType(TxNode* parent, int type);

private:
  std::map<int, Node> __NodeMap;
};


#endif // TX_FACTORY_H