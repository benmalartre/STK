
#ifndef STK_GENERATORS_H
#define STK_GENERATORS_H

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
// generator types
typedef enum STKGeneratorType{
    GENERATOR_ASYMP,
    GENERATOR_NOISE,
    GENERATOR_BLIT,
    GENERATOR_BLITSAW,
    GENERATOR_BLITSQUARE,
    GENERATOR_SINEWAVE
}STKGeneratorType;


// generator params
typedef enum STKGeneratorParam{
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


// generators union
typedef union STKGeneratorGEN
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
}STKGeneratorGEN;

// structure
typedef struct STKGenerator : public STKNode{
    STKGeneratorGEN m_g;
	std::function<StkFloat()> m_tickCallback;
	STKGeneratorType m_gentype;
	float m_frequency;
}STKGenerator;

// constructor
STKGenerator* STKGeneratorCreate(STKGeneratorType type, StkFloat frequency=440.0f);

// destructor
void STKGeneratorDelete(STKGenerator* g);

// tick function
inline StkFloat STKGeneratorTickAsymp(STKGenerator* g);
inline StkFloat STKGeneratorTickNoise(STKGenerator* g);
inline StkFloat STKGeneratorTickBlit(STKGenerator* g);
inline StkFloat STKGeneratorTickBlitSaw(STKGenerator* g);
inline StkFloat STKGeneratorTickBlitSquare(STKGenerator* g);
inline StkFloat STKGeneratorTickSineWave(STKGenerator* g);
inline StkFloat STKSingWaveTickCallback(STKGenerator* g);
inline StkFloat STKEnvelopeTickModulate(STKGenerator* g);
inline StkFloat STKEnvelopeTickGranulate(STKGenerator* g);
inline StkFloat STKGeneratorTickHasNoEffect(STKGenerator* g);
StkFloat STKGeneratorTick(STKGenerator* g, unsigned int channel = 0);


// functions
void STKGeneratorInit(STKGenerator* generator);
void STKGeneratorTerm(STKGenerator* generator);
STKGeneratorType STKGeneratorGetType(STKGenerator* generator){ return generator->m_gentype; };
void STKGeneratorSetType(STKGenerator* generator, STKGeneratorType type);
void STKGeneratorSetScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar);
void STKGenartorSetHasNoEffect(STKGenerator* generator, bool hasnoeffect);

// exported functions
EXPORT void STKSetGeneratorType(STKGenerator* generator, STKGeneratorType type);
EXPORT void STKSetGeneratorScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar);
EXPORT int STKGetGeneratorType(STKGenerator* generator);
#endif
