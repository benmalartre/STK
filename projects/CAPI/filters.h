#ifndef STK_FILTERS_H
#define STK_FILTERS_H

#include "common.h"
#include "nodes.h"
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

typedef enum STKFilterType{
    FILTER_ONEZERO,
    FILTER_ONEPOLE,
    FILTER_POLEZERO,
    FILTER_TWOZERO,
    FILTER_TWOPOLE,
    FILTER_BIQUAD,
    FILTER_FORMSWEP,
    FILTER_DELAY,
    FILTER_DELAYL,
    FILTER_DELAYA
}STKFilterType;

typedef enum STKFilterParam{
    FILTER_GAIN,            // gain
    FILTER_ONE_ZERO_ZERO,            // one zero zero
    FILTER_ONE_ZERO_B0,
    FILTER_ONE_ZERO_B1,
    FILTER_ONE_POLE_POLE,
    FILTER_ONE_POLE_B0,
    FILTER_ONE_POLE_A1,
    FILTER_TWO_ZERO_B0,
    FILTER_TWO_ZERO_B1,
    FILTER_TWO_ZERO_B2
    /*
     FILTER_B2,
     
     FILTER_TIME,            // envelope time
     FILTER_TARGET,            // envelope target
     FILTER_VALUE,            // envelope value
     FILTER_T60,            // prcrev, jcrev, nrev T60
     FILTER_MIX,            // freeverb effect mix
     FILTER_ROOMSIZE,        // freeverb room size
     FILTER_DAMPING,        // freeverb damping
     FILTER_WIDTH,            // freeverb width
     FILTER_MODE,            // freeverb mode
     FILTER_DELAY,            // echo delay
     FILTER_MAXIMUMDELAY,    // echo maximum delay
     FILTER_SHIFT,            // pitshift and lentpitshift
     FILTER_MODDEPTH,        // chorus mod depth
     FILTER_MODFREQUENCY    // chorus mod frequency
     */
}STKFilterParam;

union STKFilterFILTER
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

struct STKFilter : public STKNode{
	std::function<StkFloat()> m_tickCallback;
    STKFilterFILTER m_filter;
	STKNode* m_source;
	STKFilterType m_ftype;
};


// constructor
STKFilter* STKFilterCreate(STKNode* source, STKFilterType type);

// destructor
void STKFilterDelete(STKFilter* f);

// functions
void STKFilterReset(STKFilter* f){ f->m_outidx = 0; };
void STKFilterInit(STKFilter* f);
void STKFilterTerm(STKFilter* f);

// tick functions
inline StkFloat STKFilterTickOneZero(STKFilter* f);
inline StkFloat STKFilterTickOnePole(STKFilter* f);
inline StkFloat STKFilterTickPoleZero(STKFilter* f);
inline StkFloat STKFilterTickTwoZero(STKFilter* f);
inline StkFloat STKFilterTickTwoPole(STKFilter* f);
inline StkFloat STKFilterTickBiQuad(STKFilter* f);
inline StkFloat STKFilterTickFormSwep(STKFilter* f);
inline StkFloat STKFilterTickDelay(STKFilter* f);
inline StkFloat STKFilterTickDelayL(STKFilter* f);
inline StkFloat STKFilterTickDelayA(STKFilter* f);
inline StkFloat STKFilterTickHasNoEffect(STKFilter* f);
StkFloat STKFilterTick(STKFilter* f, unsigned int channel = 0);

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT STKFilterType STKFilterGetType(STKFilter* f){ return f->m_ftype; };
EXPORT void STKSetFilterType(STKFilter* f, STKFilterType type);
EXPORT void STKSetFilterScalar(STKFilter* f, STKFilterParam param, StkFloat scalar);

#endif
