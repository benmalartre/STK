
#ifndef STK_CAPI_COMMON_H
#define STK_CAPI_COMMON_H

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

typedef enum STKNodeType {
  STK_GENERATOR,
  STK_READER,
  STK_ARYTHMETIC,
  STK_FILTER,
  STK_EFFECT,
  STK_ENVELOPE,
  STK_INSTRUMENT,
  STK_WRITER,
  STK_BUFFER
} STKNodeType;

// generator types
typedef enum STKGeneratorType {
  GENERATOR_ASYMP,
  GENERATOR_NOISE,
  GENERATOR_BLIT,
  GENERATOR_BLITSAW,
  GENERATOR_BLITSQUARE,
  GENERATOR_SINEWAVE
}STKGeneratorType;


// generator params
typedef enum STKGeneratorParam {
  GENERATOR_T60,                    // asymp t60
  GENERATOR_TARGET,                 // asymp target
  GENERATOR_TAU,                    // asymp tau
  GENERATOR_TIME,                   // asymp time
  GENERATOR_VALUE,                  // asymp value
  GENERATOR_FREQUENCY,              // waves frequency
  GENERATOR_HARMONICS,              // waves harmonics
  GENERATOR_PHASE,                  // waves phase
  GENERATOR_PHASEOFFSET,            // sine wave phase offset
  GENERATOR_SEED,                   // noise seed
}STKGeneratorParam;

enum STKArythmeticMode {
  ARYTHMETIC_ADD,
  ARYTHMETIC_SUB,
  ARYTHMETIC_MULTIPLY,
  ARYTHMETIC_SCALE,
  ARYTHMETIC_SCALEADD,
  ARYTHMETIC_SCALESUB,
  ARYTHMETIC_MIX,
  ARYTHMETIC_BLEND,
  ARYTHMETIC_SHIFT
};

enum STKEffectType {
  EFFECT_ENVELOPE,
  EFFECT_PRCREV,
  EFFECT_JCREV,
  EFFECT_NREV,
  EFFECT_FREEVERB,
  EFFECT_ECHO,
  EFFECT_PITSHIFT,
  EFFECT_LENTPITSHIFT,
  EFFECT_CHORUS,
  EFFECT_MOOG
};

enum STKEffectParam {
  EFFECT_RATE,            // envelope rate
  EFFECT_TIME,            // envelope time
  EFFECT_TARGET,            // envelope target
  EFFECT_VALUE,            // envelope value
  EFFECT_T60,            // prcrev, jcrev, nrev T60
  EFFECT_MIX,            // freeverb effect mix
  EFFECT_ROOMSIZE,        // freeverb room size
  EFFECT_DAMPING,        // freeverb damping
  EFFECT_WIDTH,            // freeverb width
  EFFECT_MODE,            // freeverb mode
  EFFECT_DELAY,            // echo delay
  EFFECT_MAXIMUMDELAY,    // echo maximum delay
  EFFECT_SHIFT,            // pitshift and lentpitshift
  EFFECT_MODDEPTH,        // chorus/moog mod depth
  EFFECT_MODFREQUENCY    // chorus/moog mod frequency
};

enum STKEnvelopeType {
  ENVELOPE_ENVELOPE,
  ENVELOPE_ADSR,
  ENVELOPE_ASYMP
};

enum STKEnvelopeParam {
  ENVELOPE_ATTACK_RATE,
  ENVELOPE_ATTACK_TARGET,
  ENVELOPE_ATTACK_TIME,
  ENVELOPE_DECAY_RATE,
  ENVELOPE_DECAY_TIME,
  ENVELOPE_SUSTAIN_LEVEL,
  ENVELOPE_RELEASE_RATE,
  ENVELOPE_RELEASE_TIME,
  ENVELOPE_TARGET,
  ENVELOPE_VALUE,
  ENVELOPE_RATE,
  ENVELOPE_TIME
};

typedef enum STKFilterType {
  FILTER_ONEZERO,
  FILTER_ONEPOLE,
  FILTER_POLEZERO,
  FILTER_TWOZERO,
  FILTER_TWOPOLE,
  FILTER_BIQUAD,
  FILTER_FORMSWEP,
  FILTER_DELAY,
  FILTER_DELAYL,
  FILTER_DELAYA
}STKFilterType;

typedef enum STKFilterParam {
  FILTER_GAIN,            // gain
  FILTER_ONE_ZERO_ZERO,            // one zero zero
  FILTER_ONE_ZERO_B0,
  FILTER_ONE_ZERO_B1,
  FILTER_ONE_POLE_POLE,
  FILTER_ONE_POLE_B0,
  FILTER_ONE_POLE_A1,
  FILTER_TWO_ZERO_B0,
  FILTER_TWO_ZERO_B1,
  FILTER_TWO_ZERO_B2,
  /*
  FILTER_B2,

  FILTER_TIME,            // envelope time
  FILTER_TARGET,            // envelope target
  FILTER_VALUE,            // envelope value
  FILTER_T60,            // prcrev, jcrev, nrev T60
  FILTER_MIX,            // freeverb effect mix
  FILTER_ROOMSIZE,        // freeverb room size
  FILTER_DAMPING,        // freeverb damping
  FILTER_WIDTH,            // freeverb width
  FILTER_MODE,            // freeverb mode
  FILTER_DELAY,            // echo delay
  FILTER_MAXIMUMDELAY,    // echo maximum delay
  FILTER_SHIFT,            // pitshift and lentpitshift
  FILTER_MODDEPTH,        // chorus mod depth
  FILTER_MODFREQUENCY    // chorus mod frequency
  */

}STKFilterParam;

#endif // STK_CAPI_COMMON_H
