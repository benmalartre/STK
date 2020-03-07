#include "arythmetics.h"

//--------------------------------------------------------------------
// STKArythmetic Node Constructor
//--------------------------------------------------------------------
STKArythmetic* STKArythmeticCreate(STKNode* lhs, STKNode* rhs, STKArythmeticMode mode)
{
	STKArythmetic* a = new STKArythmetic();
	a->m_volume = 1.0;
	a->m_lhs = lhs;
	a->m_rhs = rhs;
	a->m_mode = mode;
	a->m_type = STK_ARYTHMETIC;
	STKArythmeticInit(a);
    return a;
}

//--------------------------------------------------------------------
// STKArythmetic Node Destructor
//--------------------------------------------------------------------
void STKArythmeticDelete(STKArythmetic* a)
{
	STKArythmeticTerm(a);
    delete a;
}


//--------------------------------------------------------------------
// STKArythmetic Node Init
//--------------------------------------------------------------------
void STKArythmeticInit(	STKArythmetic* a)
{
	if (!a->m_lhs || !a->m_rhs) return;

	switch (a->m_mode)
	{
		case ARYTHMETIC_ADD:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickAdd(a); };
			break;
		}
		case ARYTHMETIC_SUB:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickSub(a); };
			break;
		}
		case ARYTHMETIC_MULTIPLY:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickMultiply(a); };
			break;
		}
		case ARYTHMETIC_SCALE:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickScale(a); };
			break;
		}
		case ARYTHMETIC_SCALEADD:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickScaleAdd(a); };
			break;
		}
		case ARYTHMETIC_SCALESUB:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickScaleSub(a); };
			break;
		}
		case ARYTHMETIC_MIX:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickMix(a); };
			break;
		}
		case ARYTHMETIC_BLEND:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickBlend(a); };
			break;
		}
		case ARYTHMETIC_SHIFT:
		{
			a->m_tickCallback = [a](){ return STKArythmeticTickShift(a); };
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKArythmetic Node Term
//--------------------------------------------------------------------
void STKArythmeticTerm(	STKArythmetic* a)
{
}


//--------------------------------------------------------------------
// STKArythmetic Node Tick()
//--------------------------------------------------------------------
StkFloat STKArythmeticTickAdd(	STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) + STKNodeTick(a->m_rhs)) * a->m_volume;
}

StkFloat STKArythmeticTickSub(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) - STKNodeTick(a->m_rhs)) * a->m_volume ;
}

StkFloat STKArythmeticTickMultiply(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) * STKNodeTick(a->m_rhs)) * a->m_volume ;
}

StkFloat STKArythmeticTickScale(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) * a->m_scalar) * a->m_volume;
}

StkFloat STKArythmeticTickScaleAdd(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) + a->m_scalar * STKNodeTick(a->m_rhs)) * a->m_volume;
}

StkFloat STKArythmeticTickScaleSub(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) - a->m_scalar * STKNodeTick(a->m_rhs)) * a->m_volume;
}

StkFloat STKArythmeticTickMix(STKArythmetic* a)
{
	return ((1.0 - a->m_scalar) * STKNodeTick(a->m_lhs) + a->m_scalar * STKNodeTick(a->m_rhs)) * a->m_volume;
}

StkFloat STKArythmeticTickBlend(STKArythmetic* a)
{
	return ((STKNodeTick(a->m_lhs) + STKNodeTick(a->m_rhs)) * 0.5f) * a->m_volume;
}

StkFloat STKArythmeticTickShift(STKArythmetic* a)
{
	return (STKNodeTick(a->m_lhs) + a->m_scalar) * a->m_volume;
}

StkFloat STKArythmeticTickHasNoEffect(STKArythmetic* a)
{
	return STKNodeTick(a->m_lhs);
}

StkFloat STKArythmeticTick(STKArythmetic* a, unsigned int channel)
{
	if (!a->m_lhs || !a->m_rhs) return 0;
	return a->m_tickCallback();
}

void STKArythmeticSetLHS(STKArythmetic* a, STKNode* node){
	if (a->m_lhs != NULL)STKArythmeticTerm(a);
	a->m_lhs = node;
	STKArythmeticInit(a);
}

void STKArythmeticSetRHS(STKArythmetic* a, STKNode* node){
	if (a->m_rhs != NULL)STKArythmeticTerm(a);
	a->m_rhs = node;
	STKArythmeticInit(a);
}

//--------------------------------------------------------------------
// STKArythmetic Set Mode
//--------------------------------------------------------------------
void STKArythmeticSetMode(STKArythmetic* a, STKArythmeticMode mode)
{
	if (mode != a->m_mode)
	{
		STKArythmeticTerm(a);
		a->m_mode = mode;
		STKArythmeticInit(a);
	}
}

//--------------------------------------------------------------------
// STKArythmetic Set Has No Effect
//--------------------------------------------------------------------
void STKArythmeticSetHasNoEffect(STKArythmetic* a, bool hasnoeffect)
{
	if (hasnoeffect != a->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			a->m_tickCallback = [a](){return STKArythmeticTickHasNoEffect(a); };
		}
		else
		{
			STKArythmeticTerm(a);
			STKArythmeticInit(a);
		}
	}
}

void STKArythmeticSetScalar(STKArythmetic* a, StkFloat scalar)
{

}
