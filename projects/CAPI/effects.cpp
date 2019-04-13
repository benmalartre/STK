#include "effects.h"
//--------------------------------------------------------------------
// STKEffect Node Constructor
//--------------------------------------------------------------------
STKEffect::STKEffect(STKNode* source, Type type)
{
	m_source = source;
	m_volume = 1.0f;
	m_type = type;
	m_noutput = 0;
	m_outidx = 0;
	init();
}

//--------------------------------------------------------------------
// STKEffect Node Constructor
//--------------------------------------------------------------------
STKEffect::~STKEffect()
{
	term();
}

//--------------------------------------------------------------------
// STKEffect Node Init
//--------------------------------------------------------------------
void STKEffect::init()
{
	switch (m_type)
	{
		case Type::ENVELOPE:
		{
			m_envelope = new Envelope();
			m_tickCallback = [this](){ return this->EffectTickEnvelope(); };
			break;
		}
		case Type::PRCREV:
		{
			m_prcrev = new PRCRev();
			m_tickCallback = [this](){ return this->EffectTickPRCRev(); };
			break;
		}
		case Type::JCREV:
		{
			m_jcrev = new JCRev();
			m_tickCallback = [this](){ return this->EffectTickJCRev(); };
			break;
		}
		case Type::NREV:
		{
			m_nrev = new NRev();
			m_tickCallback = [this](){ return this->EffectTickNRev(); };
			break;
		}
		case Type::FREEVERB:
		{
			m_freeverb = new FreeVerb();
			m_tickCallback = [this](){ return this->EffectTickFreeVerb(); };
			break;
		}
		case Type::ECHO:
		{
			m_echo = new Echo();
			m_tickCallback = [this](){ return this->EffectTickEcho(); };
			break;
		}
		case Type::PITSHIFT:
		{
			m_pitshift = new PitShift();
			m_tickCallback = [this](){ return this->EffectTickPitShift(); };
			break;
		}
		case Type::LENTPITSHIFT:
		{
			m_lentpitshift = new LentPitShift();
			m_tickCallback = [this](){ return this->EffectTickLentPitShift(); };
			break;
		}
		case Type::CHORUS:
		{
			m_chorus = new Chorus();
			m_tickCallback = [this](){ return this->EffectTickChorus(); };
			break;
		}
		case Type::MOOG:
		{
			m_moog = new Moog();
			m_tickCallback = [this](){ return this->EffectTickMoog(); };
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKEffect Node Term
//--------------------------------------------------------------------
void STKEffect::term()
{
}

//--------------------------------------------------------------------
// STKEffect Node Tick()
//--------------------------------------------------------------------
StkFloat STKEffect::EffectTickEnvelope()
{
	if (m_outidx == 0) return update(m_envelope->tick() * m_source->tick()) * m_volume;
	else return update(m_envelope->lastOut() * m_source->tick()) * m_volume;
}

StkFloat STKEffect::EffectTickPRCRev()
{
	if (m_outidx == 0) return update(m_prcrev->tick(m_source->tick()))* m_volume;
	else return update(m_prcrev->lastOut())* m_volume;
}

StkFloat STKEffect::EffectTickJCRev()
{
	if (m_outidx == 0) return update(m_jcrev->tick(m_source->tick()))* m_volume;
	else return update(m_jcrev->lastOut())* m_volume;
}

StkFloat STKEffect::EffectTickNRev()
{
	if (m_outidx == 0) return update(m_nrev->tick(m_source->tick()))* m_volume;
	else return update(m_nrev->lastOut())* m_volume;
}

StkFloat STKEffect::EffectTickFreeVerb()
{
	if (m_outidx == 0) return update(m_freeverb->tick(m_source->tick()))* m_volume;
	else return update(m_freeverb->lastOut())* m_volume;
}

StkFloat STKEffect::EffectTickEcho()
{
	if (m_outidx == 0) return update(m_prcrev->tick(m_source->tick()))* m_volume;
	else return update(m_prcrev->lastOut())* m_volume;
	return m_echo->tick(m_source->tick())* m_volume;
}

StkFloat STKEffect::EffectTickPitShift()
{
	if (m_outidx == 0) return update(m_pitshift->tick(m_source->tick()))* m_volume;
	else return update(m_pitshift->lastOut())* m_volume;
}

StkFloat STKEffect::EffectTickLentPitShift()
{
	if (m_outidx == 0) return update(m_lentpitshift->tick(m_source->tick()))* m_volume;
	else return update(m_lentpitshift->tick(m_source->tick()))* m_volume;
}

StkFloat STKEffect::EffectTickChorus()
{
	if (m_outidx == 0) return update(m_chorus->tick(m_source->tick()))* m_volume;
	else return update(m_chorus->lastOut())* m_volume;

}

StkFloat STKEffect::EffectTickMoog()
{
	if (m_outidx == 0) return update(m_moog->tick(m_source->tick()))* m_volume;
	else return update(m_moog->lastOut())* m_volume;

}

StkFloat STKEffect::EffectTickHasNoEffect()
{
	return m_source->tick();
}

StkFloat STKEffect::tick(unsigned int channel)
{
	return m_tickCallback();
}

//--------------------------------------------------------------------
// STKEffect Node Change Type
//--------------------------------------------------------------------
void STKEffect::setType(Type type)
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
void STKEffect::setScalar( StkFloat scalar, Param param)
{
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
}

//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKEffect::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->EffectTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}

// ----------------------------------------------------------------------
//	STK EFFECT NODE SETTER
// ----------------------------------------------------------------------
void STKSetEffectType(STKEffect* effect, STKEffect::Type type)
{
	effect->setType(type);
}

void STKSetEffectScalar(STKEffect* effect, STKEffect::Param param, StkFloat scalar)
{
	effect->setScalar(scalar, param);
}