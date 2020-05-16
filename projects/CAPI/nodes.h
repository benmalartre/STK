#pragma once

#include "Stk.h"
#include "common.h"

typedef enum STKNodeType{
    NODE_GENERATOR,
    NODE_READER,
    NODE_ARYTHMETIC,
    NODE_FILTER,
    NODE_EFFECT,
    NODE_ENVELOPE,
    NODE_BUFFER,
    NODE_INSTRUMENT,
    NODE_WRITER
}STKNodeType;

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

EXPORT StkFloat STKNodeTick(STKNode* node);

EXPORT void STKNodeSetVolume(STKNode* node, StkFloat volume);
EXPORT void STKNodeIncrementNumOutput(STKNode*);
EXPORT void STKNodeDecrementNumOutputs(STKNode*);
EXPORT void STKNodeSetIsRoot(STKNode*, bool isRoot);
EXPORT bool STKNodeIsRoot(STKNode*);
EXPORT void STKNodeSetStream(STKNode*, STKStream* stream);
EXPORT STKStream* STKNodeGetStream(STKNode*);
EXPORT StkFloat STKNodeUpdate(STKNode*, StkFloat value);

