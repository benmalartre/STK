#include "common.h"
#include "factory.h"
#include "node.h"
#include "graph.h"
#include "adsr.h"
#include "lfo.h"
#include "arythmetic.h"
#include "value.h"
#include "oscillator.h"
#include "random.h"
#include "sequencer.h"
//#include "mixer.h"
#include "effect.h"
#include "filter.h"

REGISTER_NODE(TxValue, TxNode::VALUE)
REGISTER_NODE(TxRandom, TxNode::RANDOM)
REGISTER_NODE(TxOscillator, TxNode::OSCILLATOR)
REGISTER_NODE(TxLfo, TxNode::LFO)
REGISTER_NODE(TxAdsr, TxNode::ADSR)
REGISTER_NODE(TxArythmetic, TxNode::ARYTHMETIC)
REGISTER_NODE(TxFilter, TxNode::FILTER)
REGISTER_NODE(TxEffect, TxNode::EFFECT)


TxFactory& TxFactory::get() {
  static TxFactory factory;
  return factory;
}

void TxFactory::initialize()
{
  auto& constructorMap = get().__NodeMap;
  constructorMap[TxNode::VALUE] = { &(DEFINE_NODE(TxValue)), 0 };
  constructorMap[TxNode::RANDOM] = { &(DEFINE_NODE(TxRandom)), 0 };
  constructorMap[TxNode::OSCILLATOR] = { &(DEFINE_NODE(TxOscillator)), 0 };
  constructorMap[TxNode::LFO] = { &(DEFINE_NODE(TxLfo)), 0 };
  constructorMap[TxNode::ADSR] = { &(DEFINE_NODE(TxAdsr)), 0 };
  constructorMap[TxNode::ARYTHMETIC] = { &(DEFINE_NODE(TxArythmetic)), 0 };
  constructorMap[TxNode::FILTER] = { &(DEFINE_NODE(TxFilter)), 0 };
  constructorMap[TxNode::EFFECT] = { &(DEFINE_NODE(TxEffect)), 0 };
  //constructorMap[TxNode::SEQUENCER] = { &(DEFINE_NODE(TxSequencer)), 0 };
}


TxNode* TxFactory::createNodeByType(TxNode* parent, int type)
{
  std::string name = TxNode::NodeName[type];
  auto& nodeMap = get().__NodeMap;
  if (nodeMap.find(type) != nodeMap.end())
    name += std::to_string(nodeMap[type].second++);
    return nodeMap[type].first(parent, name);
  return NULL;
}