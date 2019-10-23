#ifndef STK_ARYTHMETIC_H
#define STK_ARYTHMETIC_H

#include "common.h"

class STKArythmetic : public STKNode{
public:
	enum Mode{
		ADD,
		SUB,
		MULTIPLY,
		SCALE,
		SCALEADD,
		SCALESUB,
		MIX,
		BLEND,
		SHIFT
	};
	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKArythmetic(STKNode* a, STKNode* b, Mode mode);
	// destructor 
	~STKArythmetic();
	// overrides
	void reset(){ m_outidx = 0; };
	void init();
	void term();
	// functions
	void setMode(Mode mode);
	void setScalar(StkFloat scalar){ m_scalar = scalar; };
	void setHasNoEffect(bool hasnoeffect);
	void setLHS(STKNode* node);
	void setRHS(STKNode* node);
	STKNode* getLHS(){ return m_lhs; };
	STKNode* getRHS(){ return m_rhs; };

private:
	inline StkFloat ArythmeticTickAdd();
	inline StkFloat ArythmeticTickSub();
	inline StkFloat ArythmeticTickMultiply();
	inline StkFloat ArythmeticTickScale();
	inline StkFloat ArythmeticTickScaleAdd();
	inline StkFloat ArythmeticTickScaleSub();
	inline StkFloat ArythmeticTickMix();
	inline StkFloat ArythmeticTickBlend();
	inline StkFloat ArythmeticTickShift();
	inline StkFloat ArythmeticTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	STKNode* m_lhs;
	STKNode* m_rhs;
	Mode m_mode;
	StkFloat m_scalar;
};

EXPORT void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmetic::Mode mode);
EXPORT void STKSetArythmeticScalar(STKArythmetic* arythmetic, StkFloat scalar);
EXPORT void STKSetArythmeticLHS(STKArythmetic* arythmetic, STKNode* node);
EXPORT void STKSetArythmeticRHS(STKArythmetic* arythmetic, STKNode* node);

#endif