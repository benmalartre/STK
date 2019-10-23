#ifndef STK_STREAM_H
#define STK_STREAM_H

#include "Stk.h"
#include <math.h>
#include <vector>
#include "common.h"
#include "generators.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "effects.h"
#include "filters.h"
#include "buffers.h"
#include "readers.h"

struct STKStream {
    RtAudio* m_dac;
    std::vector<STKNode*> m_nodes;
    std::vector<STKNode*> m_roots;
    STKGenerator* m_wave;
};

/*
 // Input Audio Stream
 struct STKInputStream {
 RtAudio* m_dac;
 unsigned int m_nbChannels;
 unsigned int m_sampleRate;
 unsigned int m_bufferBytes, m_nbFrames;
 RtAudio::StreamParameters m_iParams, m_oParams;
 StkFloat* m_buffer;
 float m_data[2];
 FILE* m_file;
 unsigned int m_mode;
 float m_frequency;
 float m_volume;
 bool m_initialized;
 };
 
 // VoicerStream
 struct STKVoicerStream {
 RtAudio* m_dac;
 std::vector<Instrmnt*> m_instruments;
 std::vector<Generator*> m_generators;
 int m_channels;
 int m_maxTick;
 
 Voicer* m_voicer;
 int m_nbVoices;
 bool m_enable;
 StkFloat m_frequency;
 StkFloat m_volume;
 StkFloat m_timeCurrent;
 StkFloat m_timeLast;
 int m_nbInstruments;
 StkFloat m_t60;
 int m_counter;
 bool m_settling;
 int m_currentVoice;
 int m_sampler;
 bool m_instrumentChange;
 bool m_multiInput;
 };
 */
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
