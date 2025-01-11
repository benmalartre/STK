#ifndef STK_CAPI_ENVELOPE_H
#define STK_CAPI_ENVELOPE_H
#pragma once

#include "common.h"
#include "nodes.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"

union STKEnvelopeENV
{
    Envelope* m_envelope;
    ADSR* m_adsr;
};

struct STKEnvelope : public STKNode {
  STKEnvelopeENV m_e;
  std::function<StkFloat()> m_tickCallback;
  STKEnvelopeType m_envtype;
  float m_frequency;
  STKNode* m_source;
};

// constructor
EXPORT STKEnvelope* STKEnvelopeCreate(STKEnvelopeType type, STKNode* source);

// destructor
void STKEnvelopeDelete(STKEnvelope*);

// functions
void STKEnvelopeReset(STKEnvelope*);
void STKEnvelopeInit(STKEnvelope*);
void STKEnvelopeTerm(STKEnvelope*);
 
// tick functions
StkFloat STKEnvelopeTickEnvelope(STKEnvelope*);
StkFloat STKEnvelopeTickADSR(STKEnvelope*);
StkFloat STKEnvelopeTickAsymp(STKEnvelope*);
StkFloat STKEnvelopeTickHasNoEffect(STKEnvelope*);
StkFloat STKEnvelopeTick(STKEnvelope*, unsigned int channel = 0);

// exported functions
EXPORT void STKEnvelopeSetHasNoEffect(STKEnvelope* e, bool hasnoeffect);
EXPORT STKEnvelopeType STKEnvelopeGetType(STKEnvelope* e);
EXPORT void STKEnvelopeSetType(STKEnvelope* envelope, STKEnvelopeType type);
EXPORT void STKEnvelopeSetScalar(STKEnvelope* envelope, STKEnvelopeParam param, StkFloat scalar);
EXPORT void STKEnvelopeKeyOn(STKEnvelope* envelope);
EXPORT void STKEnvelopeKeyOff(STKEnvelope* envelope);

#endif // STK_CAPI_ENVELOPE_H