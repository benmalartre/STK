#pragma once

#include "common.h"
#include "nodes.h"
#include "Envelope.h"
#include "PRCRev.h"
#include "JCRev.h"
#include "NRev.h"
#include "FreeVerb.h"
#include "Echo.h"
#include "PitShift.h"
#include "LentPitShift.h"
#include "Chorus.h"
#include "Moog.h"

enum STKEffectType{
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

enum STKEffectParam{
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

union STKEffectFX
{
    Envelope* m_envelope;
    PRCRev* m_prcrev;
    JCRev* m_jcrev;
    NRev* m_nrev;
    FreeVerb* m_freeverb;
    Echo* m_echo;
    PitShift* m_pitshift;
    LentPitShift* m_lentpitshift;
    Chorus* m_chorus;
    Moog* m_moog;
};

typedef struct STKEffect : public STKNode{
    std::function<StkFloat()> m_tickCallback;
    STKEffectFX m_fx;
	STKNode* m_source;
	STKEffectType m_fxtype;
}STKEffect;


// constructor
STKEffect* STKEffectCreate(STKNode* source, STKEffectType type);

// destructor
static void STKEffectDelete(STKEffect* fx);

// functions
static void STKEffectReset(STKEffect* fx){ fx->m_outidx = 0; };
static void STKEffectInit(STKEffect* fx);
static void STKEffectTerm(STKEffect* fx);

// tick functions
static StkFloat STKEffectTickEnvelope(STKEffect* fx);
static StkFloat STKEffectTickPRCRev(STKEffect* fx);
static StkFloat STKEffectTickJCRev(STKEffect* fx);
static StkFloat STKEffectTickNRev(STKEffect* fx);
static StkFloat STKEffectTickFreeVerb(STKEffect* fx);
static StkFloat STKEffectTickEcho(STKEffect* fx);
static StkFloat STKEffectTickPitShift(STKEffect* fx);
static StkFloat STKEffectTickLentPitShift(STKEffect* fx);
static StkFloat STKEffectTickChorus(STKEffect* fx);
static StkFloat STKEffectTickMoog(STKEffect* fx);
static StkFloat STKEffectTickHasNoEffect(STKEffect* fx);
static StkFloat STKEffectTick(STKEffect* fx, unsigned int channel = 0);

// functions
static STKEffectType STKEffectGetType(STKEffect* fx);
static void STKEffectSetType(STKEffect* fx, STKEffectType type);
static void STKEffectSetScalar(STKEffect* fx, StkFloat scalar, STKEffectParam param);
static void STKEffectSetHasNoEffect(STKEffect* fx, bool hasnoeffect);


// ----------------------------------------------------------------------
//	STK EFFECT NODE SETTER
// ----------------------------------------------------------------------
EXPORT void STKSetEffectType(STKEffect* effect, STKEffectType type);
EXPORT void STKSetEffectScalar(STKEffect* effect, STKEffectParam param, StkFloat scalar);
