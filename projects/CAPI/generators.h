
#ifndef STK_GENERATORS_H
#define STK_GENERATORS_H

#include "Stk.h"
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
#include "nodes.h"

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
union STKGeneratorGEN
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
};
// structure
struct STKGenerator : public STKNode{
    STKGeneratorGEN m_generator;
	std::function<StkFloat()> m_tickCallback;
	STKGeneratorType m_gentype;
	float m_frequency;
};
// constructor
STKGenerator* STKGeneratorCreate(STKGeneratorType type, StkFloat frequency=440.0f);

// destructor
void STKGeneratorDelete(STKGenerator* generator);

// inline function
inline StkFloat STKGeneratorTickAsymp();
inline StkFloat STKGeneratorTickNoise();
inline StkFloat STKGeneratorTickBlit();
inline StkFloat STKGeneratorTickBlitSaw();
inline StkFloat STKGeneratorTickBlitSquare();
inline StkFloat STKGeneratorTickSineWave();
inline StkFloat STKSingWaveTickCallback();
inline StkFloat STKEnvelopeTickModulate();
inline StkFloat STKEnvelopeTickGranulate();
inline StkFloat STKGeneratorTickHasNoEffect();
StkFloat STKGeneratorTick(unsigned int channel = 0);


// functions
void STKGeneratorInit(STKGenerator* generator);
void STKGeneratorTerm(STKGenerator* generator);
STKGeneratorType STKGeneratorGetType(STKGenerator* generator){ return generator->m_type; };
void STKGeneratorSetType(STKGenerator* generator, STKGeneratorType type);
void STKGeneratorSetScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar);
void STKGenartorSetHasNoEffect(STKGenerator* generator, bool hasnoeffect);

// exported functions
EXPORT void STKSetGeneratorType(STKGenerator* generator, STKGenerator::Type type);
EXPORT void STKSetGeneratorScalar(STKGenerator* generator, STKGenerator::Param param, StkFloat scalar);
EXPORT int STKGetGeneratorType(STKGenerator* generator);
#endif
