
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

#define FORMAT RTAUDIO_FLOAT32

typedef enum STKNodeType{
    STK_GENERATOR,
    STK_READER,
    STK_ARYTHMETIC,
    STK_FILTER,
    STK_EFFECT,
    STK_ENVELOPE,
    STK_INSTRUMENT,
    STK_WRITER,
    STK_BUFFER
}STKNodeType;
#endif
