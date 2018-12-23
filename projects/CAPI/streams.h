#ifndef STK_STREAM_H
#define STK_STREAM_H

#include "common.h"
#include "generators.h"
#include "envelopes.h"
#include "arythmetics.h"
#include "effects.h"
#include "filters.h"

struct STKGeneratorStream {
	RtAudio* m_dac;
	std::vector<STKNode*> m_nodes;
	std::vector<STKNode*> m_roots;
	STKGenerator* m_wave;
};

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

// InputStream tick() function
int STKInputStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKInputStream* STKInputStreamInit(RtAudio* dac, const char* filename);
EXPORT bool STKInputStreamGet(STKInputStream* stream);
EXPORT void STKInputStreamTerm(STKInputStream* stream);
EXPORT bool STKInputStreamSetFile(STKInputStream* stream);

// GeneratorStream tick() function
int STKGeneratorStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKGeneratorStream* STKGeneratorStreamSetup(RtAudio* dac);
EXPORT bool STKGeneratorStreamClean(STKGeneratorStream* generator);
EXPORT bool STKGeneratorStreamStart(STKGeneratorStream* generator);
EXPORT bool STKGeneratorStreamStop(STKGeneratorStream* generator);
EXPORT void STKGeneratorStreamSetFrequency(STKGeneratorStream* generator, float frequency);

EXPORT STKNode* STKAddGenerator(STKGeneratorStream* generator, STKGenerator::Type type, float frequency, bool isRoot = true);
EXPORT STKNode* STKAddEnvelope(STKGeneratorStream* generator, STKEnvelope::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddArythmetic(STKGeneratorStream* generator, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKNode* STKAddEffect(STKGeneratorStream* generator, STKEffect::Type type, STKNode* source, bool isRoot = true);
EXPORT STKNode* STKAddFilter(STKGeneratorStream* generator, STKFilter::Type type, STKNode* source, bool isRoot = true);

// VoicerStream tick() function
int STKVoicerStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKVoicerStream* STKVoicerStreamSetup(RtAudio* dac, int nbInstruments);
EXPORT bool STKVoicerStreamClean(STKVoicerStream* voicer);
EXPORT bool STKVoicerStreamStart(STKVoicerStream* voicer);
EXPORT bool STKVoicerStreamStop(STKVoicerStream* voicer);

class STKGeneratorMixer {
	std::vector<STKGeneratorStream*> m_generators;
};

#endif