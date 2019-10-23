#ifndef STK_STREAM_H
#define STK_STREAM_H

#include "Stk.h"
#include <math.h>
#include "generators.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "effects.h"
#include "filters.h"
#include "buffers.h"
#include "readers.h"

struct STKNode;
typedef struct STKStream {
	RtAudio* m_dac;
    std::vector<STKNode*> m_roots;
}STKStream;

EXPORT STKStreamRemoveRootNode(STKStream* stream, STKNode* node);
EXPORT STKStreamAddRootNode(STKStream* stream, STKNode* node);

// GeneratorStream tick() function
int STKStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKStream* STKStreamSetup(RtAudio* dac, int numChannels);
EXPORT bool STKStreamClean(STKStream* stream);
EXPORT bool STKStreamStart(STKStream* stream);
EXPORT bool STKStreamStop(STKStream* stream);
EXPORT void STKStreamSetFrequency(STKStream* stream, float frequency);
EXPORT int STKStreamNumRoots(STKStream* stream);

EXPORT STKNode* STKStreamAddGenerator(STKStream* stream, STKGenerator::Type type, float frequency, bool isRoot = true);
EXPORT STKNode* STKStreamAddEnvelope(STKStream* stream, STKEnvelope::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddArythmetic(STKStream* stream, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKNode* STKStreamAddEffect(STKStream* stream, STKEffect::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddFilter(STKStream* stream, STKFilter::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddBuffer(STKStream* stream, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddReader(STKStream* stream, const char* filename, bool isRoot = true);


#endif
