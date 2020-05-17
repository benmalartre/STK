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
STKArythmetic* STKArythmeticCreate(STKNode* a, STKNode* b, STKArythmeticMode mode);

// destructor
void STKArythmeticDelete(STKArythmetic* a);

// functions
inline void STKArythmeticReset(STKArythmetic* a){ a->m_outidx = 0; };
void STKArythmeticInit(STKArythmetic* a);
void STKArythmeticTerm(STKArythmetic* a);
void STKArythmeticSetHasNoEffect(STKArythmetic* a, bool hasnoeffect);
void STKArythmeticSetLHS(STKArythmetic* a, STKNode* node);
void STKArythmeticSetRHS(STKArythmetic* a, STKNode* node);
inline STKNode* STKArythmeticGetLHS(STKArythmetic* a){ return a->m_lhs; };
inline STKNode* STKArythmeticGetRHS(STKArythmetic* a){ return a->m_rhs; };

// tick functions
inline StkFloat STKArythmeticTickAdd(STKArythmetic* a);
inline StkFloat STKArythmeticTickSub(STKArythmetic* a);
inline StkFloat STKArythmeticTickMultiply(STKArythmetic* a);
inline StkFloat STKArythmeticTickScale(STKArythmetic* a);
inline StkFloat STKArythmeticTickScaleAdd(STKArythmetic* a);
inline StkFloat STKArythmeticTickScaleSub(STKArythmetic* a);
inline StkFloat STKArythmeticTickMix(STKArythmetic* a);
inline StkFloat STKArythmeticTickBlend(STKArythmetic* a);
inline StkFloat STKArythmeticTickShift(STKArythmetic* a);
inline StkFloat STKArythmeticTickHasNoEffect(STKArythmetic* a);

StkFloat STKArythmeticTick(STKArythmetic* a, unsigned int channel = 0);

// export functions
EXPORT void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmeticMode mode);
EXPORT void STKSetArythmeticScalar(STKArythmetic* arythmetic, StkFloat scalar);
EXPORT void STKSetArythmeticLHS(STKArythmetic* arythmetic, STKNode* node);
EXPORT void STKSetArythmeticRHS(STKArythmetic* arythmetic, STKNode* node);

#endif
