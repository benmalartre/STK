#pragma once

#include "common.h"
#include "nodes.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"

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

union STKEnvelopeENV
{
    Envelope* m_envelope;
    ADSR* m_adsr;
};

struct STKEnvelope : public STKNode{
    STKEnvelopeENV m_e;
	std::function<StkFloat()> m_tickCallback;
	STKEnvelopeType m_envtype;
	float m_frequency;
	STKNode* m_source;

};

// constructor
static STKEnvelope* STKEnvelopeCreate(STKEnvelopeType type, STKNode* source);

// destructor
static void STKEnvelopeDelete(STKEnvelope*);

// functions
static void STKEnvelopeReset(STKEnvelope*);
static void STKEnvelopeInit(STKEnvelope*);
static void STKEnvelopeTerm(STKEnvelope*);
 
// tick functions
static StkFloat STKEnvelopeTickEnvelope(STKEnvelope*);
static StkFloat STKEnvelopeTickADSR(STKEnvelope*);
static StkFloat STKEnvelopeTickAsymp(STKEnvelope*);
static StkFloat STKEnvelopeTickHasNoEffect(STKEnvelope*);
static StkFloat STKEnvelopeTick(STKEnvelope*, unsigned int channel = 0);

// exported functions
EXPORT void STKEnvelopeSetHasNoEffect(STKEnvelope* e, bool hasnoeffect);
EXPORT STKEnvelopeType STKEnvelopeGetType(STKEnvelope* e);
EXPORT void STKEnvelopeSetType(STKEnvelope* envelope, STKEnvelopeType type);
EXPORT void STKEnvelopeSetScalar(STKEnvelope* envelope, STKEnvelopeParam param, StkFloat scalar);
EXPORT void STKEnvelopeKeyOn(STKEnvelope* envelope);
EXPORT void STKEnvelopeKeyOff(STKEnvelope* envelope);
