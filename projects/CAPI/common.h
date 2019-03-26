
#ifndef STK_COMMON_H
#define STK_COMMON_H 
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

#include <functional>
#include <vector>

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
protected:
	StkFloat update(StkFloat value);
	bool m_hasnoeffect;
	bool m_isroot;
	int m_noutput;
	int m_outidx;
};

// General functions
EXPORT RtAudio* STKInit();
EXPORT void STKTerm(RtAudio* DAC);
EXPORT void STKSetSampleRate(float rate);
EXPORT int STKGetDevices(RtAudio* DAC);

EXPORT void STKSetNodeHasNoEffect(STKNode* node, bool hasnoeffect);

#endif