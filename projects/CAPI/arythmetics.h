#ifndef STK_ARYTHMETIC_H
#define STK_ARYTHMETIC_H

#pragma once

#include "common.h"
#include "nodes.h"

enum STKArythmeticMode{
	ARYTHMETIC_ADD,
	ARYTHMETIC_SUB,
	ARYTHMETIC_MULTIPLY,
	ARYTHMETIC_SCALE,
	ARYTHMETIC_SCALEADD,
	ARYTHMETIC_SCALESUB,
	ARYTHMETIC_MIX,
	ARYTHMETIC_BLEND,
	ARYTHMETIC_SHIFT
};

struct STKArythmetic : public STKNode{
	std::function<StkFloat()> m_tickCallback;
	STKNode* m_lhs;
	STKNode* m_rhs;
	STKArythmeticMode m_mode;
	StkFloat m_scalar;
};

// constructor
EXPORT STKArythmetic* STKArythmeticCreate(STKNode* a, STKNode* b, STKArythmeticMode mode);

// destructor
static void STKArythmeticDelete(STKArythmetic* a);

// functions
static void STKArythmeticReset(STKArythmetic* a){ a->m_outidx = 0; };
static void STKArythmeticInit(STKArythmetic* a);
static void STKArythmeticTerm(STKArythmetic* a);
static void STKArythmeticSetHasNoEffect(STKArythmetic* a, bool hasnoeffect);
static void STKArythmeticSetLHS(STKArythmetic* a, STKNode* node);
static void STKArythmeticSetRHS(STKArythmetic* a, STKNode* node);
static STKNode* STKArythmeticGetLHS(STKArythmetic* a){ return a->m_lhs; };
static STKNode* STKArythmeticGetRHS(STKArythmetic* a){ return a->m_rhs; };

// tick functions
static inline StkFloat STKArythmeticTickAdd(STKArythmetic* a);
static inline StkFloat STKArythmeticTickSub(STKArythmetic* a);
static inline StkFloat STKArythmeticTickMultiply(STKArythmetic* a);
static inline StkFloat STKArythmeticTickScale(STKArythmetic* a);
static inline StkFloat STKArythmeticTickScaleAdd(STKArythmetic* a);
static inline StkFloat STKArythmeticTickScaleSub(STKArythmetic* a);
static inline StkFloat STKArythmeticTickMix(STKArythmetic* a);
static inline StkFloat STKArythmeticTickBlend(STKArythmetic* a);
static inline StkFloat STKArythmeticTickShift(STKArythmetic* a);
static inline StkFloat STKArythmeticTickHasNoEffect(STKArythmetic* a);

StkFloat STKArythmeticTick(STKArythmetic* a, unsigned int channel = 0);

// export functions
EXPORT void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmeticMode mode);
EXPORT void STKSetArythmeticScalar(STKArythmetic* arythmetic, StkFloat scalar);
EXPORT void STKSetArythmeticLHS(STKArythmetic* arythmetic, STKNode* node);
EXPORT void STKSetArythmeticRHS(STKArythmetic* arythmetic, STKNode* node);

#endif
