#ifndef STK_ENVELOPES_H
#define STK_ENVELOPES_H

#include "common.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"

/*

*/
class STKEnvelope : public STKNode{
public:
	enum Type {
		ENVELOPE_ENVELOPE,
		ENVELOPE_ADSR,
		ENVELOPE_ASYMP
	};

	enum Param {
		ENV_ATTACK_RATE,
		ENV_ATTACK_TARGET,
		ENV_ATTACK_TIME,
		ENV_DECAY_RATE,
		ENV_DECAY_TIME,
		ENV_SUSTAIN_LEVEL,
		ENV_RELEASE_RATE,
		ENV_RELEASE_TIME,
		ENV_TARGET,
		ENV_VALUE,
		ENV_RATE,
		ENV_TIME
	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKEnvelope(Type type, STKNode* source);
	// destructor 
	~STKEnvelope();
	// overrides
	void reset();
	void init();
	void term();
	// functions
	Type getType(){ return m_type; };
	void setType(Type type);
	void setScalar(Param param, StkFloat scalar);
	void setHasNoEffect(bool hasnoeffect);
	void keyOn();
	void keyOff();


private:
	union
	{
		Envelope* m_envelope;
		ADSR* m_adsr;
	};
	inline StkFloat EnvelopeTickEnvelope();
	inline StkFloat EnvelopeTickADSR();
	inline StkFloat EnvelopeTickAsymp();
	inline StkFloat EnvelopeTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	Type m_type;
	float m_frequency;
	STKNode* m_source;

};

EXPORT void STKSetEnvelopeType(STKEnvelope* envelope, STKEnvelope::Type type);
EXPORT void STKSetEnvelopeScalar(STKEnvelope* envelope, STKEnvelope::Param param, StkFloat scalar);
EXPORT void STKEnvelopeKeyOn(STKEnvelope* envelope);
EXPORT void STKEnvelopeKeyOff(STKEnvelope* envelope);

#endif