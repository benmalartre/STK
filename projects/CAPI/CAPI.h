#include "SineWave.h"
#include "Generator.h"
#include "Instrmnt.h"
#include "Voicer.h"
#include "RtAudio.h"

using namespace stk;

#if defined(_WIN32) || defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 

#if defined(STATIC_LIB)
#define EXPORT extern "C" 
#else
#define EXPORT extern "C" __declspec(dllexport)
#endif
#else // Unix variants
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#define EXPORT extern "C" 
#endif

// Global Constants
#define FORMAT RTAUDIO_FLOAT32

// GeneratorStream
enum STKGeneratorType {
	ENVELOPE_GENERATOR, 
	ADSR_GENERATOR,
	ASYMP_GENERATOR, 
	NOISE_GENERATOR, 
	MODULATE_GENERATOR, 
	SINGWAVE_GENERATOR, 
	SINEWAVE_GENERATOR, 
	BLIT_GENERATOR, 
	BLITSAW_GENERATOR, 
	BLITSQUARE_GENERATOR, 
	GRANULATE_GENERATOR
};

struct STKGeneratorStream {
	RtAudio* m_dac;
	Generator* m_generator;
	STKGeneratorType m_type;
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

// General functions
EXPORT RtAudio* STKInit();
EXPORT void STKTerm(RtAudio* DAC);
EXPORT void STKSetSampleRate(StkFloat _rate);
EXPORT int STKGetDevices(RtAudio* DAC);

// InputStream tick() function
int STKInputStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKInputStream* STKInputStreamInit(RtAudio* dac, const char* filename);
EXPORT bool STKInputStreamGet(STKInputStream* _stream);
EXPORT void STKInputStreamTerm(STKInputStream* _stream);
EXPORT bool STKInputStreamSetFile(STKInputStream* _stream);

// GeneratorStream tick() function
int STKGeneratorStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKGeneratorStream*  STKGeneratorStreamSetup(RtAudio* dac, STKGeneratorType _type, float _frequency);
EXPORT bool STKGeneratorStreamClean(STKGeneratorStream* _generator);
EXPORT bool STKGeneratorStreamStart(STKGeneratorStream* _generator);
EXPORT bool STKGeneratorStreamStop(STKGeneratorStream* _generator);
EXPORT void STKGeneratorStreamSetFrequency(STKGeneratorStream* _generator, float _frequency);

// VoicerStream tick() function
int STKVoicerStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer);

EXPORT STKVoicerStream* STKVoicerStreamSetup(RtAudio* dac, int _nbInstruments);
EXPORT bool STKVoicerStreamClean(STKVoicerStream* _voicer);
EXPORT bool STKVoicerStreamStart(STKVoicerStream* _voicer);
EXPORT bool STKVoicerStreamStop(STKVoicerStream* _voicer);

class STKGeneratorMixer {
	std::vector<STKGeneratorStream*> m_generators;

};