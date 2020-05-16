#pragma once

#include "common.h"
#include <math.h>
#include "generators.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "effects.h"
#include "filters.h"
#include "buffers.h"

struct STKNode;
struct STKReader;

typedef struct STKStream {
	RtAudio* m_dac;
    std::vector<STKNode*> m_roots;
}STKStream;

EXPORT void STKStreamRemoveRootNode(STKStream* stream, STKNode* node);
EXPORT void STKStreamAddRootNode(STKStream* stream, STKNode* node);

// GeneratorStream tick() function
static int STKStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKStream* STKStreamSetup(RtAudio* dac, int numChannels);
EXPORT bool STKStreamClean(STKStream* stream);
EXPORT bool STKStreamStart(STKStream* stream);
EXPORT bool STKStreamStop(STKStream* stream);
EXPORT void STKStreamSetFrequency(STKStream* stream, float frequency);
EXPORT int STKStreamNumRoots(STKStream* stream);

EXPORT STKNode* STKStreamAddGenerator(STKStream* stream, STKGeneratorType type, float frequency, bool isRoot = true);
EXPORT STKNode* STKStreamAddEnvelope(STKStream* stream, STKEnvelopeType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddArythmetic(STKStream* stream, STKArythmeticMode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKNode* STKStreamAddEffect(STKStream* stream, STKEffectType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddFilter(STKStream* stream, STKFilterType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddBuffer(STKStream* stream, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddReader(STKStream* stream, const char* filename, bool isRoot = true);
