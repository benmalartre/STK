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

StkFloat STKNodeUpdate(STKNode* node, StkFloat value)
{
    node->m_outidx = (node->m_outidx + 1) % node->m_noutput;

    return value;
}

void STKNodeIncrementNumOutput(STKNode* node){
    node->m_noutput++;
}

void STKNodeDecrementNumOutput(STKNode* node){
    node->m_noutput--;
}

void STKNodeSetIsRoot(STKNode* node, bool isRoot){
    node->m_isroot = isRoot;
    if(node->m_stream ){
        if(!isRoot)STKStreamRemoveRootNode(node->m_stream, node);
        else STKStreamAddRootNode(node->m_stream, node);
    }
}

bool STKNodeIsRoot(STKNode* node){ return node->m_isroot; };

void STKNodeSetStream(STKNode* node, STKStream* stream) { node->m_stream = stream; };

STKStream* STKNodeGetStream(STKNode* node){ return node->m_stream; };

StkFloat STKNodeTick(STKNode* node){
  switch(node->m_type){
    case STK_GENERATOR:
    {
      STKGeneratorTick((STKGenerator*)node);
      break;
    }
    case STK_READER:
    {
      STKReaderTick((STKReader*)node);
      break;
    }
    case STK_EFFECT:
    {
      STKEffectTick((STKEffect*)node);
      break;
    }
    case STK_FILTER:
    {
      STKFilterTick((STKFilter*)node);
      break;
    }
    /*
    case NODE_INSTRUMENT:
    {
        STKInstrumentTick((STKInstrument*)node);
        break;
    }
    */
    case STK_WRITER:
    {
      STKWriterTick((STKWriter*)node);
    }
  }
  return 0.f;
}

