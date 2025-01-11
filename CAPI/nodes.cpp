#include "nodes.h"
#include "streams.h"
#include "writers.h"

//--------------------------------------------------------------------
// STKNode Set Volume
//--------------------------------------------------------------------
void STKNodeSetVolume(STKNode* node, StkFloat volume)
{
  node->m_volume = volume;
}

void STKNodeIncrementNumOutputs(STKNode* node){
    node->m_noutput++;
}

void STKNodeDecrementNumOutput(STKNode* node){
    node->m_noutput--;
}

void STKNodeSetIsRoot(STKNode* node, bool isRoot){
    node->m_isroot = isRoot;
    if(node->m_stream ) {
      STKStreamAddNode(node->m_stream, node, isRoot);
    }
}

bool STKNodeIsRoot(STKNode* node){ return node->m_isroot; };

void STKNodeSetStream(STKNode* node, STKStream* stream) { node->m_stream = stream; };
