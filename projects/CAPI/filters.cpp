#include "filters.h"
//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
STKFilter::STKFilter(STKNode* source, Type type)
{
	m_source = source;
	m_type = type;
	m_noutput = 0;
	m_outidx = 0;
	init();
}

//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
STKFilter::~STKFilter()
{
	term();
}

//--------------------------------------------------------------------
// STKFilter Node Init
//--------------------------------------------------------------------
void STKFilter::init()
{
	switch (m_type)
	{
		case Type::ONEZERO:
		{
			m_onezero = new OneZero();
			m_tickCallback = [this](){ return this->FilterTickOneZero(); };
			break;
		}
		case Type::POLEZERO:
		{
			m_polezero = new PoleZero();
			m_tickCallback = [this](){ return this->FilterTickPoleZero(); };
			break;
		}
		case Type::TWOZERO:
		{
			m_twozero = new TwoZero();
			m_tickCallback = [this](){ return this->FilterTickTwoZero(); };
			break;
		}
		case Type::TWOPOLE:
		{
			m_twopole = new TwoPole();
			m_tickCallback = [this](){ return this->FilterTickTwoPole(); };
			break;
		}
		case Type::BIQUAD:
		{
			m_biquad = new BiQuad();
			m_tickCallback = [this](){ return this->FilterTickBiQuad(); };
			break;
		}
		case Type::FORMSWEP:
		{
			m_formswep = new FormSwep();
			m_tickCallback = [this](){ return this->FilterTickFormSwep(); };
			break;
		}
		case Type::DELAY:
		{
			m_delay = new Delay();
			m_tickCallback = [this](){ return this->FilterTickDelay(); };
			break;
		}
		case Type::DELAYA:
		{
			m_delaya = new DelayA();
			m_tickCallback = [this](){ return this->FilterTickDelayA(); };
			break;
		}
		case Type::DELAYL:
		{
			m_delayl = new DelayL();
			m_tickCallback = [this](){ return this->FilterTickDelayL(); };
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKFilter Node Term
//--------------------------------------------------------------------
void STKFilter::term()
{
}


//--------------------------------------------------------------------
// STKFilter Node Tick()
//--------------------------------------------------------------------
StkFloat STKFilter::FilterTickOneZero()
{
	if (m_outidx == 0) return update(m_onezero->tick(m_source->tick()));
	else return update(m_onezero->lastOut());
}

StkFloat STKFilter::FilterTickOnePole()
{
	if (m_outidx == 0) return update(m_onepole->tick(m_source->tick()));
	else return update(m_onepole->lastOut());
}

StkFloat STKFilter::FilterTickPoleZero()
{
	if (m_outidx == 0) return update(m_polezero->tick(m_source->tick()));
	else return update(m_polezero->lastOut());
}

StkFloat STKFilter::FilterTickTwoZero()
{
	if (m_outidx == 0) return update(m_twozero->tick(m_source->tick()));
	else return update(m_twozero->lastOut());
}

StkFloat STKFilter::FilterTickTwoPole()
{
	if (m_outidx == 0) return update(m_twopole->tick(m_source->tick()));
	else return update(m_twopole->lastOut());
}

StkFloat STKFilter::FilterTickBiQuad()
{
	if (m_outidx == 0) return update(m_biquad->tick(m_source->tick()));
	else return update(m_biquad->lastOut());
}

StkFloat STKFilter::FilterTickFormSwep()
{
	if (m_outidx == 0) return update(m_formswep->tick(m_source->tick()));
	else return update(m_formswep->lastOut());
}

StkFloat STKFilter::FilterTickDelay()
{
	if (m_outidx == 0) return update(m_delay->tick(m_source->tick()));
	else return update(m_delay->lastOut());
}

StkFloat STKFilter::FilterTickDelayA()
{
	if (m_outidx == 0) return update(m_delaya->tick(m_source->tick()));
	else return update(m_delaya->lastOut());
}

StkFloat STKFilter::FilterTickDelayL()
{
	if (m_outidx == 0) return update(m_delayl->tick(m_source->tick()));
	else return update(m_delayl->lastOut());
}

StkFloat STKFilter::FilterTickHasNoEffect()
{
	return m_source->tick();
}

StkFloat STKFilter::tick(unsigned int channel)
{
	return m_tickCallback();
}

//--------------------------------------------------------------------
// STKEffect Node Change Type
//--------------------------------------------------------------------
void STKFilter::setType(Type type)
{
	if (type != m_type){
		term();
		m_type = type;
		init();
	}
}

//--------------------------------------------------------------------
// STKEffect Node Change Scalar
//--------------------------------------------------------------------
void STKFilter::setScalar(StkFloat scalar, Param param)
{
	/*
	switch (m_type){
		case ENVELOPE:
		{
			if (param == RATE)
			{
				if (scalar < 0)scalar = 0;
				m_envelope->setRate(scalar);
			}
			else if (param == TIME)m_envelope->setTime(scalar);
			else if (param == TARGET)m_envelope->setTarget(scalar);
			else if (param == VALUE)m_envelope->setValue(scalar);
			break;
		}
		case PRCREV:
		{
			if (param == T60)m_prcrev->setT60(scalar);
			else if (param == MIX)m_prcrev->setEffectMix(scalar);
		}
		case JCREV:
		{
			if (param == T60)m_jcrev->setT60(scalar);
			else if (param == MIX)m_jcrev->setEffectMix(scalar);
		}
		case NREV:
		{
			if (param == T60)m_nrev->setT60(scalar);
			else if (param == MIX)m_nrev->setEffectMix(scalar);
		}
		case FREEVERB:
		{
			if (param == MIX)m_freeverb->setEffectMix(scalar);
			else if (param == ROOMSIZE)m_freeverb->setRoomSize(scalar);
			else if (param == DAMPING)m_freeverb->setDamping(scalar);
			else if (param == WIDTH)m_freeverb->setWidth(scalar);
			else if (param == MODE)m_freeverb->setMode((bool)scalar);
		}
		case ECHO:
		{
			if (param == DELAY)m_echo->setDelay(scalar);
			else if (param == MAXIMUMDELAY)m_echo->setMaximumDelay(scalar);
			else if (param == MIX)m_echo->setEffectMix(scalar);
		}
		case PITSHIFT:
		{
			if (param == SHIFT)m_pitshift->setShift(scalar);
			else if (param == MIX)m_pitshift->setEffectMix(scalar);
		}
		case LENTPITSHIFT:
		{
			if (param == SHIFT)m_lentpitshift->setShift(scalar);
			else if (param == MIX)m_lentpitshift->setEffectMix(scalar);
		}
		case CHORUS:
		{
			if (param == MODDEPTH)m_chorus->setModDepth(scalar);
			else if (param == MODFREQUENCY)m_chorus->setModFrequency(scalar);
			else if (param == MIX)m_chorus->setEffectMix(scalar);
		}
	}
	*/
}

//--------------------------------------------------------------------
// STKFilter Set Has No Effect
//--------------------------------------------------------------------
void STKFilter::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->FilterTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
void STKSetFilterType(STKFilter* filter, STKFilter::Type type)
{
	filter->setType(type);
}

void STKSetFilterScalar(STKFilter* filter, STKFilter::Param param, StkFloat scalar)
{
	filter->setScalar(scalar, param);
}