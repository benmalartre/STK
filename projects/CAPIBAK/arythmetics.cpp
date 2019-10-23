#include "arythmetics.h"

//--------------------------------------------------------------------
// STKArythmetic Node Constructor
//--------------------------------------------------------------------
STKArythmetic::STKArythmetic(STKNode* a, STKNode* b, Mode mode)
{
	m_volume = 1.0;
	m_lhs = a;
	m_rhs = b;
	m_mode = mode;
	init();
}

//--------------------------------------------------------------------
// STKArythmetic Node Constructor
//--------------------------------------------------------------------
STKArythmetic::~STKArythmetic()
{
    term();
}

//--------------------------------------------------------------------
// STKArythmetic Set Mode
//--------------------------------------------------------------------
void STKArythmetic::setMode(Mode mode)
{
	if (mode != m_mode)
	{
		term();
		m_mode = mode;
		init();
	}
}

//--------------------------------------------------------------------
// STKArythmetic Set Has No Effect
//--------------------------------------------------------------------
void STKArythmetic::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->ArythmeticTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}


//--------------------------------------------------------------------
// STKArythmetic Node Init
//--------------------------------------------------------------------
void STKArythmetic::init()
{
	if (!this->m_lhs || !this->m_rhs) return;

	switch (m_mode)
	{
		case Mode::ADD:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickAdd(); };
			break;
		}
		case Mode::SUB:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickSub(); };
			break;
		}
		case Mode::MULTIPLY:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickMultiply(); };
			break;
		}
		case Mode::SCALE:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScale(); };
			break;
		}
		case Mode::SCALEADD:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScaleAdd(); };
			break;
		}
		case Mode::SCALESUB:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScaleSub(); };
			break;
		}
		case Mode::MIX:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickMix(); };
			break;
		}
		case Mode::BLEND:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickBlend(); };
			break;
		}
		case Mode::SHIFT:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickShift(); };
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKArythmetic Node Term
//--------------------------------------------------------------------
void STKArythmetic::term()
{
}


//--------------------------------------------------------------------
// STKArythmetic Node Tick()
//--------------------------------------------------------------------
StkFloat STKArythmetic::ArythmeticTickAdd()
{
	return (m_lhs->tick() + m_rhs->tick()) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickSub()
{
	return (m_lhs->tick() - m_rhs->tick()) * m_volume ;
}

StkFloat STKArythmetic::ArythmeticTickMultiply()
{
	return (m_lhs->tick() * m_rhs->tick()) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickScale()
{
	return (m_lhs->tick() * m_scalar) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickScaleAdd()
{
	return (m_lhs->tick() + m_scalar * m_rhs->tick()) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickScaleSub()
{
	return (m_lhs->tick() - m_scalar * m_rhs->tick()) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickMix()
{
	return ((1.0 - m_scalar) * m_lhs->tick() + m_scalar * m_rhs->tick()) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickBlend()
{
	return ((m_lhs->tick() + m_rhs->tick()) * 0.5f) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickShift()
{
	return (m_lhs->tick() + m_scalar) * m_volume;
}

StkFloat STKArythmetic::ArythmeticTickHasNoEffect()
{
	return m_lhs->tick();
}

StkFloat STKArythmetic::tick(unsigned int channel)
{
	if (!m_lhs || !m_rhs) return 0;
	return m_tickCallback();
}

void STKArythmetic::setLHS(STKNode* node){
	if (m_lhs != NULL)term();
	m_lhs = node;
	init();
}

void STKArythmetic::setRHS(STKNode* node){
	if (m_rhs != NULL)term();
	m_rhs = node;
	init();
}

// ----------------------------------------------------------------------
//	STK GENERATOR ARYTHMETIC SETTER
// ----------------------------------------------------------------------
void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmetic::Mode mode)
{
	arythmetic->setMode(mode);
}

void STKSetArythmeticScalar(STKArythmetic* generator, StkFloat scalar)
{
	generator->setScalar(scalar);
}

void STKSetArythmeticLHS(STKArythmetic* arythmetic, STKNode* node)
{
	arythmetic->setLHS(node);
}

void STKSetArythmeticRHS(STKArythmetic* arythmetic, STKNode* node)
{
	arythmetic->setRHS(node);
}
