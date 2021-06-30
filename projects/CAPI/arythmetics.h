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

struct STKArythmetic : public STKNode {
  STKNode* m_lhs;
  STKNode* m_rhs;
  STKArythmeticMode m_mode;
  StkFloat m_scalar;
};

// constructor
EXPORT STKArythmetic* STKArythmeticCreate(STKNode* a, STKNode* b, STKArythmeticMode mode);

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

EXPORT StkFloat STKArythmeticTick(STKArythmetic* a, unsigned int channel = 1);

// export functions
EXPORT void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmeticMode mode);
EXPORT void STKSetArythmeticScalar(STKArythmetic* arythmetic, StkFloat scalar);
EXPORT void STKSetArythmeticLhs(STKArythmetic* arythmetic, STKNode* node);
EXPORT void STKSetArythmeticRhs(STKArythmetic* arythmetic, STKNode* node);

#endif
