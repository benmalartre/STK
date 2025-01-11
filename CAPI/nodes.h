#ifndef STK_CAPI_NODE_H
#define STK_CAPI_NODE_H

#include "Stk.h"
#include "common.h"

struct STKStream;

struct STKNode {
  STKStream* m_stream;
  STKNodeType m_type;
  bool m_hasnoeffect;
  bool m_isroot;
  int m_noutput;
  int m_outidx;
  StkFloat m_volume;
    
  std::function<StkFloat()> m_tickCallback;
};

EXPORT void STKNodeSetVolume(STKNode* node, StkFloat volume);
EXPORT void STKNodeIncrementNumOutputs(STKNode*);
EXPORT void STKNodeDecrementNumOutputs(STKNode*);
EXPORT void STKNodeSetIsRoot(STKNode*, bool isRoot);
EXPORT bool STKNodeIsRoot(STKNode*);
EXPORT void STKNodeSetStream(STKNode*, STKStream* stream);
EXPORT inline STKStream* STKNodeGetStream(STKNode* node)
{
  return node->m_stream;
}

EXPORT inline StkFloat STKNodeUpdate(STKNode* node, StkFloat value)
{
  node->m_outidx = (node->m_outidx + 1) % node->m_noutput;
  return value;
}

EXPORT inline StkFloat STKNodeTick(STKNode* node)
{
  return node->m_tickCallback();
}
#endif //STK_CAPI_NODE_H