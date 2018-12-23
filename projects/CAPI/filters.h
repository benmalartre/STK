#ifndef STK_FILTERS_H
#define STK_FILTERS_H

#include "common.h"
#include "Filter.h"
#include "OneZero.h"
#include "OnePole.h"
#include "PoleZero.h"
#include "TwoZero.h"
#include "TwoPole.h"
#include "BiQuad.h"
#include "FormSwep.h"
#include "Delay.h"
#include "DelayL.h"
#include "DelayA.h"


class STKFilter : public STKNode{
public:
	enum Type{
		ONEZERO,
		ONEPOLE,
		POLEZERO,
		TWOZERO,
		TWOPOLE,
		BIQUAD,
		FORMSWEP,
		DELAY,
		DELAYL,
		DELAYA
	};

	enum Param{
		GAIN,			// gain
		ONE_ZERO_ZERO,			// one zero zero
		ONE_ZERO_B0,
		ONE_ZERO_B1,
		ONE_POLE_POLE,
		ONE_POLE_B0,
		ONE_POLE_A1,
		TWO_ZERO_B0,
		TWO_ZERO_B1,
		TWO_ZERO_B2
		/*
		B2,

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
		MODDEPTH,		// chorus mod depth	
		MODFREQUENCY	// chorus mod frequency
		*/
	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKFilter(STKNode* source, Type type);
	// destructor 
	~STKFilter();
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
	inline StkFloat FilterTickOneZero();
	inline StkFloat FilterTickOnePole();
	inline StkFloat FilterTickPoleZero();
	inline StkFloat FilterTickTwoZero();
	inline StkFloat FilterTickTwoPole();
	inline StkFloat FilterTickBiQuad();
	inline StkFloat FilterTickFormSwep();
	inline StkFloat FilterTickDelay();
	inline StkFloat FilterTickDelayL();
	inline StkFloat FilterTickDelayA();
	inline StkFloat FilterTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	union
	{
		OneZero* m_onezero;
		OnePole* m_onepole;
		PoleZero* m_polezero;
		TwoZero* m_twozero;
		TwoPole* m_twopole;
		BiQuad* m_biquad;
		FormSwep* m_formswep;
		Delay* m_delay;
		DelayL* m_delayl;
		DelayA* m_delaya;
		LentPitShift* m_lentpitshift;
		Chorus* m_chorus;
	};
	STKNode* m_source;
	Type m_type;
};

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT void STKSetFilterType(STKFilter* filter, STKFilter::Type type);
EXPORT void STKSetFilterScalar(STKFilter* filter, STKFilter::Param param, StkFloat scalar);

#endif