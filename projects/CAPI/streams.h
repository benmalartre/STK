#ifndef STK_CAPI_STREAM_H
#define STK_CAPI_STREAM_H

#include <math.h>
#include <vector>
#include "common.h"

//#include <map>
typedef struct STKStream {
  RtAudio* m_dac;
  short m_channels;
  std::vector<STKNode*> m_nodes;
  std::vector<STKNode*> m_roots;
} STKStream;
struct STKNode;

EXPORT void STKStreamAddNode(STKStream* stream, STKNode* node, bool isRoot);
EXPORT void STKStreamRemoveNode(STKStream* stream, STKNode* node);
EXPORT void STKStreamRemoveAllNodes(STKStream* stream);

EXPORT STKStream* STKStreamSetup(RtAudio* DAC, int numChannels);

// GeneratorStream tick() function
static int STKStreamTick(void *outputBuffer, void *inputBuffer, 
  unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, 
  void *dataPointer);

EXPORT STKStream* STKStreamSetup(RtAudio* dac, int numChannels);
EXPORT bool STKStreamClean(STKStream* stream);
EXPORT bool STKStreamStart(STKStream* stream);
EXPORT bool STKStreamStop(STKStream* stream);
EXPORT int STKStreamNumRoots(STKStream* stream);

EXPORT STKNode* STKStreamAddGenerator(STKStream* stream, 
  STKGeneratorType type, float frequency, bool isRoot=true);
EXPORT STKNode* STKStreamAddEnvelope(STKStream* stream, 
  STKEnvelopeType type, STKNode* source, bool isRoot=true);
EXPORT STKNode* STKStreamAddArythmetic(STKStream* stream, 
  STKArythmeticMode mode, STKNode* lhs, STKNode* rhs, bool isRoot=true);
EXPORT STKNode* STKStreamAddEffect(STKStream* stream, 
  STKEffectType type, STKNode* source, bool isRoot=true);
EXPORT STKNode* STKStreamAddFilter(STKStream* stream, 
  STKFilterType type, STKNode* source, bool isRoot=true);
EXPORT STKNode* STKStreamAddBuffer(STKStream* stream, 
  STKNode* source, bool isRoot=true);
EXPORT STKNode* STKStreamAddReader(STKStream* stream, 
  const char* filename, bool isRoot=true);

#endif // STK_CAPI_STREAM_H
