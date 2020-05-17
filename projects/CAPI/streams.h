#ifndef STK_CAPI_STREAM_H
#define STK_CAPI_STREAM_H
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

struct STKStream {
    RtAudio* m_dac;
    std::vector<STKNode*> m_roots;
    stk::SineWave* m_sinewave;
};

EXPORT void STKStreamRemoveRootNode(STKStream* stream, STKNode* node);
EXPORT void STKStreamAddRootNode(STKStream* stream, STKNode* node);

int STKStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKStream* STKStreamSetup(RtAudio* dac, int numChannels);
EXPORT bool STKStreamClean(STKStream* stream);
EXPORT bool STKStreamStart(STKStream* stream);
EXPORT bool STKStreamStop(STKStream* stream);
EXPORT int STKStreamNumRoots(STKStream* stream);

EXPORT STKNode* STKStreamAddGenerator(STKStream* stream, STKGeneratorType type, float frequency, bool isRoot = true);
EXPORT STKNode* STKStreamAddEnvelope(STKStream* stream, STKEnvelopeType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddArythmetic(STKStream* stream, STKArythmeticMode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKNode* STKStreamAddEffect(STKStream* stream, STKEffectType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddFilter(STKStream* stream, STKFilterType type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddBuffer(STKStream* stream, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKStreamAddReader(STKStream* stream, const char* filename, bool isRoot = true);

#endif // STK_CAPI_STREAM_H
