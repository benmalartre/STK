#include "Generator.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"
#include "Noise.h"
#include "Modulate.h"
#include "SingWave.h"
#include "SineWave.h"
#include "Blit.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Granulate.h"
#include "Instrmnt.h"
#include "Voicer.h"
#include "Effect.h"
#include "Echo.h"
#include "PitShift.h"
#include "LentPitShift.h"
#include "Chorus.h"
#include "RtAudio.h"

#include <functional>

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

class STKNode{
public:
	virtual StkFloat tick(unsigned int channel = 0)=0;
	virtual void reset() = 0;
	virtual void init() = 0;
	virtual void term() = 0;
};


class STKGenerator : public STKNode{
public:
	enum Type {
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

	typedef StkFloat(*GeneratorTickCallback)();
	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKGenerator(Type type, StkFloat frequency=440.0f);
	// destructor 
	~STKGenerator();
	// setter
	void setFrequency(float frequency);
	void reset();
	Type getType(){ return m_type; };
	void setType(Type type);
	void init();
	void term();

private:

	union
	{
		Envelope* m_envelope;
		ADSR* m_adsr;
		Asymp* m_asymp;
		Noise* m_noise;
		Modulate* m_modulate;
		SingWave* m_singwave;
		SineWave* m_sinewave;
		Blit* m_blit;
		BlitSaw* m_blitsaw;
		BlitSquare* m_blitsquare;
		Granulate* m_granulate;
	};
	inline StkFloat EnvelopeTickCallback();
	inline StkFloat ADSRTickCallback();
	inline StkFloat AsympTickCallback();
	inline StkFloat NoiseTickCallback();
	inline StkFloat ModulateTickCallback();
	inline StkFloat SingWaveTickCallback();
	inline StkFloat SineWaveTickCallback();
	inline StkFloat BlitTickCallback();
	inline StkFloat BlitSawTickCallback();
	inline StkFloat BlitSquareTickCallback();
	inline StkFloat GranulateTickCallback();
	std::function<StkFloat()> m_tickCallback;
	Type m_type;
	float m_frequency;
	
};

class STKArythmetic : public STKNode{
public:
	enum Mode{
		ADD,
		SUB,
		MULTIPLY,
		SCALE,
		SCALEADD,
		SCALESUB,
		MIX,
		BLEND
	};
	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKArythmetic(STKNode* a, STKNode* b, Mode mode);
	// destructor 
	~STKArythmetic();
	// overrides
	void reset(){};
	void init();
	void term();
	// functions
	void setMode(Mode mode);
	void setScalar(StkFloat scalar){ m_scalar = scalar; };

private:
	inline StkFloat ArythmeticTickAdd();
	inline StkFloat ArythmeticTickSub();
	inline StkFloat ArythmeticTickMultiply();
	inline StkFloat ArythmeticTickScale();
	inline StkFloat ArythmeticTickScaleAdd();
	inline StkFloat ArythmeticTickScaleSub();
	inline StkFloat ArythmeticTickMix();
	inline StkFloat ArythmeticTickBlend();
	std::function<StkFloat()> m_tickCallback;
	STKNode* m_lhs;
	STKNode* m_rhs;
	Mode m_mode;
	StkFloat m_scalar;
};

class STKEffect : public STKNode{
public:
	enum Type{
		ENVELOPE,
		PRCREV,
		JCREV,
		NREV,
		FREEVERB,
		ECHO,
		PITSHIFT,
		LENTPITSHIFT,
		CHORUS
	};
	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKEffect(STKNode* source, Type type){
		m_source = source;
		m_type = type;
	}
	// destructor 
	~STKEffect();
	// overrides
	void reset(){};
	void init(){};
	void term(){};
private:
	Effect* m_effect;
	STKNode* m_source;
	Type m_type;
};

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

// General functions
EXPORT RtAudio* STKInit();
EXPORT void STKTerm(RtAudio* DAC);
EXPORT void STKSetSampleRate(float rate);
EXPORT int STKGetDevices(RtAudio* DAC);

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

EXPORT STKGeneratorStream* STKGeneratorStreamSetup(RtAudio* dac, STKGenerator::Type type, float frequency);
EXPORT bool STKGeneratorStreamClean(STKGeneratorStream* generator);
EXPORT bool STKGeneratorStreamStart(STKGeneratorStream* generator);
EXPORT bool STKGeneratorStreamStop(STKGeneratorStream* generator);
EXPORT void STKGeneratorStreamSetFrequency(STKGeneratorStream* generator, float frequency);

EXPORT STKGenerator* STKAddGenerator(STKGeneratorStream* generator, STKGenerator::Type type, float frequency, bool isRoot = true);
EXPORT STKArythmetic* STKAddArythmetic(STKGeneratorStream* generator, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot = true);
EXPORT STKEffect* STKAddEffect(STKGeneratorStream* generator, STKEffect::Type type, STKNode* source, bool isRoot = true);

EXPORT void STKSetGeneratorType(STKGenerator* generator, STKGenerator::Type type);
EXPORT void STKSetArythmeticMode(STKArythmetic* generator, STKArythmetic::Mode mode);
EXPORT void STKSetArythmeticScalar(STKArythmetic* generator, StkFloat scalar);
EXPORT void STKSetEffectType(STKEffect* effect, STKEffect::Type type);


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