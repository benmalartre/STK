
#ifndef STK_COMMON_H
#define STK_COMMON_H

#include <functional>
#include <vector>

#include "Stk.h"
#include "Generator.h"
#include "Instrmnt.h"
#include "Voicer.h"
#include "Effect.h"
#include "Echo.h"
#include "PitShift.h"
#include "LentPitShift.h"
#include "Chorus.h"
#include "RtAudio.h"

#include "nodes.h"
#include "streams.h"
#include "nodes.h"
#include "generators.h"
#include "readers.h"

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

enum STKNodeType{
    GENERATOR,
    READER,
    FILTER,
    EFFECT,
    INSTRUMENT,
    WRITER
}

// exported functions
EXPORT RtAudio* STKInit();
EXPORT void STKTerm(RtAudio* DAC);
EXPORT void STKSetSampleRate(float rate);
EXPORT int STKGetDevices(RtAudio* DAC);


EXPORT StkFloat STKNodeTick(STKNode*);
EXPORT void STKNodeSetHasNoEffect(STKNode* node, bool hasnoeffect);
EXPORT void STKNodeReset(STKNode*);
EXPORT void STKNodeInit(STKNode*);
EXPORT void STKNodeTerm(STKNode*)



/*
class STKStream;
// base class
class STKNode{
public:
	virtual StkFloat tick(unsigned int channel = 0) = 0;
	virtual void reset() = 0;
	virtual void init() = 0;
	virtual void term() = 0;
	virtual void setHasNoEffect(bool hasnoeffect) = 0;
	void incrementNumOutput(){ m_noutput++; };
	void decrementNumOutput(){ m_noutput--; };
	void setIsRoot(bool isRoot) { m_isroot = isRoot; };
	bool isRoot(){ return m_isroot; };
	void setStream(STKStream* stream) { m_stream = stream; };
	void setVolume(StkFloat volume){ m_volume = volume; };
	STKStream* getStream(){ return m_stream; };
protected:
    StkFloat update(StkFloat value);
	bool m_hasnoeffect;
	bool m_isroot;
	int m_noutput;
	int m_outidx;
	StkFloat m_volume;
	STKStream* m_stream;
    STKNodeType m_type;

};
*/

#endif
