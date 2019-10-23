#ifndef STK_EFFECTS_H
#define STK_EFFECTS_H
#include "common.h"
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

class STKEffect : public STKNode{
public:
	enum Type{
		ENVELOPE,
		PRCREV,
		JCREV,
		NREV,
		FREEVERB,
		ECHO,
		PITSHIFT,
		LENTPITSHIFT,
		CHORUS,
		MOOG
	};

	enum Param{
		RATE,			// envelope rate
		TIME,			// envelope time
		TARGET,			// envelope target
		VALUE,			// envelope value
		T60,			// prcrev, jcrev, nrev T60
		MIX,			// freeverb effect mix
		ROOMSIZE,		// freeverb room size
		DAMPING,		// freeverb damping
		WIDTH,			// freeverb width
		MODE,			// freeverb mode
		DELAY,			// echo delay
		MAXIMUMDELAY,	// echo maximum delay
		SHIFT,			// pitshift and lentpitshift 
		MODDEPTH,		// chorus/moog mod depth	
		MODFREQUENCY	// chorus/moog mod frequency
	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKEffect(STKNode* source, Type type);
	// destructor 
	~STKEffect();
	// overrides
	void reset(){ m_outidx = 0; };
	void init();
	void term();
	// functions
	Type getType(){ return m_type; };
	void setType(Type type);
	void setScalar(StkFloat scalar, Param param);
	void setHasNoEffect(bool hasnoeffect);
private:
	inline StkFloat EffectTickEnvelope();
	inline StkFloat EffectTickPRCRev();
	inline StkFloat EffectTickJCRev();
	inline StkFloat EffectTickNRev();
	inline StkFloat EffectTickFreeVerb();
	inline StkFloat EffectTickEcho();
	inline StkFloat EffectTickPitShift();
	inline StkFloat EffectTickLentPitShift();
	inline StkFloat EffectTickChorus();
	inline StkFloat EffectTickMoog();
	inline StkFloat EffectTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	union
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
	STKNode* m_source;
	Type m_type;
};

// ----------------------------------------------------------------------
//	STK EFFECT NODE SETTER
// ----------------------------------------------------------------------
EXPORT void STKSetEffectType(STKEffect* effect, STKEffect::Type type);
EXPORT void STKSetEffectScalar(STKEffect* effect, STKEffect::Param param, StkFloat scalar);

#endif