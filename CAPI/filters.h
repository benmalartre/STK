#ifndef STK_CAPI_FILTER_H
#define STK_CAPI_FILTER_H
#pragma once

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
    STKFilterFILTER m_filter;
    STKNode* m_source;
    STKFilterType m_ftype;
};


// constructor
EXPORT STKFilter* STKFilterCreate(STKNode* source, STKFilterType type);

// destructor
void STKFilterDelete(STKFilter* f);

// functions
inline void STKFilterReset(STKFilter* f){ f->m_outidx = 0; };
void STKFilterInit(STKFilter* f);
void STKFilterTerm(STKFilter* f);

// tick functions
static StkFloat STKFilterTickOneZero(STKFilter* f);
static StkFloat STKFilterTickOnePole(STKFilter* f);
static StkFloat STKFilterTickPoleZero(STKFilter* f);
static StkFloat STKFilterTickTwoZero(STKFilter* f);
static StkFloat STKFilterTickTwoPole(STKFilter* f);
static StkFloat STKFilterTickBiQuad(STKFilter* f);
static StkFloat STKFilterTickFormSwep(STKFilter* f);
static StkFloat STKFilterTickDelay(STKFilter* f);
static StkFloat STKFilterTickDelayL(STKFilter* f);
static StkFloat STKFilterTickDelayA(STKFilter* f);
static StkFloat STKFilterTickHasNoEffect(STKFilter* f);
EXPORT StkFloat STKFilterTick(STKFilter* f, unsigned int channel = 0);

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT STKFilterType STKFilterGetType(STKFilter* f);
EXPORT void STKSetFilterType(STKFilter* f, STKFilterType type);
EXPORT void STKSetFilterScalar(STKFilter* f, STKFilterParam param, StkFloat scalar);

#endif // STK_CAPI_FILTER_H
