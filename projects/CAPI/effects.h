#ifndef STK_CAPI_EFFECT_H
#define STK_CAPI_EFFECT_H
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
    STKEffectFX m_fx;
    STKNode* m_source;
    STKEffectType m_fxtype;
}STKEffect;


// constructor
STKEffect* STKEffectCreate(STKNode* source, STKEffectType type);

// destructor
void STKEffectDelete(STKEffect* fx);

// functions
inline void STKEffectReset(STKEffect* fx){ fx->m_outidx = 0; };
void STKEffectInit(STKEffect* fx);
void STKEffectTerm(STKEffect* fx);

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
EXPORT StkFloat STKEffectTick(STKEffect* fx, unsigned int channel = 0);

// functions
STKEffectType STKEffectGetType(STKEffect* fx);
void STKEffectSetType(STKEffect* fx, STKEffectType type);
void STKEffectSetScalar(STKEffect* fx, StkFloat scalar, STKEffectParam param);
void STKEffectSetHasNoEffect(STKEffect* fx, bool hasnoeffect);


// ----------------------------------------------------------------------
//	STK EFFECT NODE SETTER
// ----------------------------------------------------------------------
EXPORT void STKSetEffectType(STKEffect* effect, STKEffectType type);
EXPORT void STKSetEffectScalar(STKEffect* effect, STKEffectParam param, StkFloat scalar);

#endif // STK_CAPI_EFFECT_H
