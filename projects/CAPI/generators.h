#ifndef STK_CAPI_GENERATOR_H
#define STK_CAPI_GENERATOR_H
#pragma once

#include "common.h"
#include "nodes.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"
#include "Noise.h"
#include "SingWave.h"
#include "SineWave.h"
#include "Blit.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Modulate.h"
#include "Granulate.h"

using namespace stk;


// generators union
typedef union STKGeneratorImpl
{
    stk::Asymp* m_asymp;
    stk::Noise* m_noise;
    stk::Blit* m_blit;
    stk::BlitSaw* m_blitsaw;
    stk::BlitSquare* m_blitsquare;
    stk::SineWave* m_sinewave;
    stk::SingWave* m_singwave;
    stk::Modulate* m_modulate;
    stk::Granulate* m_granulate;
}STKGeneratorImpl;

// structure
struct STKGenerator : public STKNode{
    STKGeneratorImpl m_g;
    STKGeneratorType m_gentype;
    float m_frequency;
};

// constructor
EXPORT STKGenerator* STKGeneratorCreate(STKGeneratorType type, StkFloat frequency=440.0f);

// destructor
void STKGeneratorDelete(STKGenerator* g);

// tick function
static inline StkFloat STKGeneratorTickAsymp(STKGenerator* g);
static inline StkFloat STKGeneratorTickNoise(STKGenerator* g);
static inline StkFloat STKGeneratorTickBlit(STKGenerator* g);
static inline StkFloat STKGeneratorTickBlitSaw(STKGenerator* g);
static inline StkFloat STKGeneratorTickBlitSquare(STKGenerator* g);
static inline StkFloat STKGeneratorTickSineWave(STKGenerator* g);
static inline StkFloat STKSingWaveTickCallback(STKGenerator* g);
static inline StkFloat STKEnvelopeTickModulate(STKGenerator* g);
static inline StkFloat STKEnvelopeTickGranulate(STKGenerator* g);
static inline StkFloat STKGeneratorTickHasNoEffect(STKGenerator* g);
EXPORT StkFloat STKGeneratorTick(STKGenerator* g, unsigned int channel = 0);


// functions
void STKGeneratorInit(STKGenerator* generator);
void STKGeneratorTerm(STKGenerator* generator);
inline STKGeneratorType STKGeneratorGetType(STKGenerator* generator){ return generator->m_gentype; };
void STKGeneratorSetType(STKGenerator* generator, STKGeneratorType type);
void STKGeneratorSetScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar);
void STKGeneratorSetHasNoEffect(STKGenerator* generator, bool hasnoeffect);

// exported functions
EXPORT void STKSetGeneratorType(STKGenerator* generator, STKGeneratorType type);
EXPORT void STKSetGeneratorScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar);
EXPORT int STKGetGeneratorType(STKGenerator* generator);

#endif // STK_CAPI_GENERATOR_H
