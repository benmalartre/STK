#ifndef STK_STREAM_H
#define STK_STREAM_H

#include "Stk.h"
#include <math.h>
#include <vector>
#include <map>
#include "common.h"
#include "generators.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "effects.h"
#include "filters.h"
#include "buffers.h"
#include "readers.h"

class STKStream {
public:
	RtAudio* m_dac;
	std::vector<STKNode*> m_roots;
    std::vector<STKNode*> m_nodes;
    
    /*
    void addNode(STKNode* node);
    void addRoot(STKNode* node);
    */
};

EXPORT bool STKIsRoot(STKNode* node);
EXPORT void STKSetAsRoot(STKNode* node, bool isRoot);
EXPORT STKStream* STKGetStream(STKNode* node);

// GeneratorStream tick() function
int STKStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKStream* STKStreamSetup(RtAudio* dac, int numChannels);
EXPORT bool STKStreamClean(STKStream* stream);
EXPORT bool STKStreamStart(STKStream* stream);
EXPORT bool STKStreamStop(STKStream* stream);
EXPORT void STKStreamSetFrequency(STKStream* stream, float frequency);
EXPORT int STKStreamNumRoots(STKStream* stream);

EXPORT STKNode* STKAddGenerator(STKStream* stream, STKGenerator::Type type, float frequency, bool isRoot = true);
EXPORT STKNode* STKAddEnvelope(STKStream* stream, STKEnvelope::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddArythmetic(STKStream* stream, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKNode* STKAddEffect(STKStream* stream, STKEffect::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddFilter(STKStream* stream, STKFilter::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddBuffer(STKStream* stream, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddReader(STKStream* stream, const char* filename, bool isRoot = true);

EXPORT void STKRemoveNode(STKStream* stream, STKNode* node);
EXPORT void STKRemoveAllNodes(STKStream* stream);

#endif
