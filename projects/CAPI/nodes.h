#ifndef STK_NODE_H
#define STK_NODE_H

#include "Stk.h"
#include "common.h"

struct STKStream;

struct STKNode{
    bool m_hasnoeffect;
    bool m_isroot;
    int m_noutput;
    int m_outidx;
    StkFloat m_volume;
    STKStream* m_stream;
    STKNodeType m_type;
    
};

StkFloat STKNodeTick(STKNode* node);

EXPORT void STKNodeSetVolume(STKNode* node, StkFloat volume);
EXPORT void STKNodeIncrementNumOutput(STKNode*);
EXPORT void STKNodeDecrementNumOutputs(STKNode*);
EXPORT void STKNodeSetIsRoot(STKNode*, bool isRoot);
EXPORT bool STKNodeIsRoot(STKNode*);
EXPORT void STKNodeSetStream(STKNode*, STKStream* stream);
EXPORT STKStream* STKNodeGetStream(STKNode*);
EXPORT StkFloat STKNodeUpdate(STKNode*, StkFloat value);

#endif
