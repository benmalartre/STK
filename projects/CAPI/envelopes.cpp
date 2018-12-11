#include "envelopes.h"

//--------------------------------------------------------------------
// STKEnvelope Node Constructor
//--------------------------------------------------------------------
STKEnvelope::STKEnvelope(Type type, STKNode* source)
{
	m_noutput = 0;
	m_outidx = 0;
	m_type = type;
	m_source = source;
	init();
}

//--------------------------------------------------------------------
// STKEnvelope Node Terminate
//--------------------------------------------------------------------
void STKEnvelope::term()
{
	if (m_adsr)delete m_adsr;
	if (m_envelope)delete m_envelope;
}

//--------------------------------------------------------------------
// STKEnvelope Node Init
//--------------------------------------------------------------------
void STKEnvelope::init()
{
	switch (m_type)
	{
		case ADSR_GENERATOR:
		{
			m_adsr = new ADSR();
			m_adsr->setValue(m_frequency);
			m_type = ADSR_GENERATOR;
			m_tickCallback = [this](){ return this->EnvelopeTickADSR(); };
			break;
		}

		case ENVELOPE_GENERATOR:
		{
			m_envelope = new Envelope();
			m_type = ENVELOPE_GENERATOR;
			m_tickCallback = [this](){ return this->EnvelopeTickEnvelope(); };
			break;
		}

	}
}


//--------------------------------------------------------------------
// STKEnvelope Node Destructor
//--------------------------------------------------------------------
STKEnvelope::~STKEnvelope()
{
	term();
}

//--------------------------------------------------------------------
// STKEnvelope Node Reset
//--------------------------------------------------------------------
void STKEnvelope::reset()
{
	switch (m_type)
	{
		case ENVELOPE_GENERATOR:
		{
			m_envelope->setValue(0);
			break;
		}
		case ADSR_GENERATOR:
		{
			m_adsr->setValue(0);
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Type
//--------------------------------------------------------------------
void STKEnvelope::setType(Type type)
{
	if (type != m_type)
	{
		term();
		m_type = type;
		init();
	}
}

//--------------------------------------------------------------------
// STKEnvelope KeyOn
//--------------------------------------------------------------------
void STKEnvelope::keyOn()
{
	switch(m_type)
	{
		case ENVELOPE_GENERATOR:
		{
			m_envelope->keyOn();
			break;
		}
		case ADSR_GENERATOR:
		{
			m_adsr->keyOn();
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKEnvelope KeyOff
//--------------------------------------------------------------------
void STKEnvelope::keyOff()
{
	switch (m_type)
	{
		case ENVELOPE_GENERATOR:
		{
			m_envelope->keyOff();
			break;
		}
		case ADSR_GENERATOR:
		{
			m_adsr->keyOff();
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Scalar
//--------------------------------------------------------------------
void STKEnvelope::setScalar(Param param, StkFloat scalar)
{
	switch (m_type){
		case ADSR_GENERATOR:
		{
			if (param == ATTACK_RATE)m_adsr->setAttackRate(scalar);
			else if (param == ATTACK_TARGET)m_adsr->setAttackTarget(scalar);
			else if (param == ATTACK_TIME)m_adsr->setAttackTime(scalar);
			else if (param == DECAY_RATE)m_adsr->setDecayRate(scalar);
			else if (param == DECAY_TIME)m_adsr->setDecayTime(scalar);
			else if (param == SUSTAIN_LEVEL)m_adsr->setSustainLevel(scalar);
			else if (param == RELEASE_RATE)m_adsr->setReleaseRate(scalar);
			else if (param == RELEASE_TIME)m_adsr->setReleaseTime(scalar);
			else if (param == TARGET)m_adsr->setTarget(scalar);
			else if (param == VALUE)m_adsr->setValue(scalar);
		}
		case ENVELOPE_GENERATOR:
		{
			if (param == RATE)m_envelope->setRate(scalar);
			else if (param == TIME)m_envelope->setTime(scalar);
			else if (param == TARGET)m_envelope->setTarget(scalar);
			else if (param == VALUE)m_envelope->setValue(scalar);
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Has No Effect
//--------------------------------------------------------------------
void STKEnvelope::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->EnvelopeTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Node Tick()
//--------------------------------------------------------------------
StkFloat STKEnvelope::EnvelopeTickEnvelope()
{
	if (m_outidx==0)return update(m_source->tick() * m_envelope->tick());
	else return update(m_source->tick() + m_envelope->lastOut());
}
StkFloat STKEnvelope::EnvelopeTickADSR()
{
	if (m_outidx == 0)return update(m_source->tick() * m_adsr->tick());
	else return update(m_source->tick() + m_adsr->lastOut());
}

StkFloat STKEnvelope::EnvelopeTickHasNoEffect()
{
	return 0;
}
StkFloat STKEnvelope::tick(unsigned int channel)
{
	return m_tickCallback();
}

// ----------------------------------------------------------------------
//	SETTERS
// ----------------------------------------------------------------------
void STKSetEnvelopeType(STKEnvelope* envelope, STKEnvelope::Type type)
{
	envelope->setType(type);
}

void STKSetEnvelopeScalar(STKEnvelope* envelope, STKEnvelope::Param param, StkFloat scalar)
{
	envelope->setScalar(param, scalar);
}

void STKEnvelopeKeyOn(STKEnvelope* envelope)
{
	envelope->keyOn();
}

void STKEnvelopeKeyOff(STKEnvelope* envelope)
{
	envelope->keyOff();
}