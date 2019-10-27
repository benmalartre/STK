#include "envelopes.h"

//--------------------------------------------------------------------
// STKEnvelope Node Constructor
//--------------------------------------------------------------------
STKEnvelope* STKEnvelopeCreate(STKEnvelopeType type, STKNode* source)
{
    STKEnvelope* e = new STKEnvelope();
	e->m_noutput = 0;
	e->m_volume = 1.0f;
	e->m_outidx = 0;
	e->m_type = type;
	e->m_source = source;
	STKEnvelopeInit(e);
    return e;
}

//--------------------------------------------------------------------
// STKEnvelope Node Destructor
//--------------------------------------------------------------------
void STKEnvelopeDelete(STKEnvelope* e)
{
    STKEnvelopeTerm(e);
    delete e;
}
//--------------------------------------------------------------------
// STKEnvelope Node Terminate
//--------------------------------------------------------------------
void STKEnvelopeTerm(STKEnvelope* e)
{
	if (e->m_e.m_adsr)delete e->m_e.m_adsr;
	if (e->m_e.m_envelope)delete e->m_e.m_envelope;
}

//--------------------------------------------------------------------
// STKEnvelope Node Init
//--------------------------------------------------------------------
void STKEnvelopeInit(STKEnvelope* e)
{
	switch (e->m_type)
	{
		case ENVELOPE_ADSR:
		{
			e->m_e.m_adsr = new ADSR();
			e->m_e.m_adsr->setValue(e->m_frequency);
			e->m_e.m_type = ENVELOPE_ADSR;
			e->m_tickCallback = [e](){ return STKEnvelopeTickADSR(e); };
			break;
		}

		case ENVELOPE_ENVELOPE:
		{
			e->m_e.m_envelope = new Envelope();
			e->m_type = ENVELOPE_ENVELOPE;
			m_tickCallback = [e](){ return STKEnvelopeTickEnvelope(e); };
			break;
		}

	}
}

//--------------------------------------------------------------------
// STKEnvelope Node Reset
//--------------------------------------------------------------------
void STKEnvelopeReset(STKEnvelope* e)
{
	switch (e->m_type)
	{
	case ENVELOPE_ENVELOPE:
		{
			e->m_e.m_envelope->setValue(0);
			break;
		}
	case ENVELOPE_ADSR:
		{
			e->m_e.m_adsr->setValue(0);
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Type
//--------------------------------------------------------------------
void STKEnvelopeSetType(STKEnvelope* e, STKEnvelopeType type)
{
	if (type != e->m_type)
	{
		STKEnvelopeTerm(e);
		e->m_type = type;
		STKEnvelopeInit(e);
	}
}

//--------------------------------------------------------------------
// STKEnvelope KeyOn
//--------------------------------------------------------------------
void STKEnvelopeKeyOn(STKEnvelope* e)
{
	switch(e->m_type)
	{
	    case ENVELOPE_ENVELOPE:
		{
			e->m_e.m_envelope->keyOn();
			break;
		}
	    case ENVELOPE_ADSR:
		{
			e->m_e.m_adsr->keyOn();
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKEnvelope KeyOff
//--------------------------------------------------------------------
void STKEnvelopeKeyOff(STKEnvelope* e)
{
	switch (e->m_type)
	{
	case ENVELOPE_ENVELOPE:
		{
			e->m_e.m_envelope->keyOff();
			break;
		}
	case ENVELOPE_ADSR:
		{
			e->m_e.m_adsr->keyOff();
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Scalar
//--------------------------------------------------------------------
void STKEnvelopeSetScalar(STKEnvelope* e, STKEnvelopeParam param, StkFloat scalar)
{
	switch (e->m_type){
		case ENVELOPE_ADSR:
		{
			if (param == ENVELOPE_ATTACK_RATE)e->m_e.m_adsr->setAttackRate(scalar);
			else if (param == ENVELOPE_ATTACK_TARGET)e->m_e.m_adsr->setAttackTarget(scalar);
			else if (param == ENVELOPE_ATTACK_TIME)e->m_e.m_adsr->setAttackTime(scalar);
			else if (param == ENVELOPE_DECAY_RATE)e->m_e.m_adsr->setDecayRate(scalar);
			else if (param == ENVELOPE_DECAY_TIME)e->m_e.m_adsr->setDecayTime(scalar);
			else if (param == ENVELOPE_SUSTAIN_LEVEL)e->m_e.m_adsr->setSustainLevel(scalar);
			else if (param == ENVELOPE_RELEASE_RATE)e->m_e.m_adsr->setReleaseRate(scalar);
			else if (param == ENVELOPE_RELEASE_TIME)e->m_e.m_adsr->setReleaseTime(scalar);
			else if (param == ENVELOPE_TARGET)e->m_e.m_adsr->setTarget(scalar);
			else if (param == ENVELOPE_VALUE)e->m_e.m_adsr->setValue(scalar);
		}
		case ENVELOPE_ENVELOPE:
		{
			if (param == ENVELOPE_RATE)e->m_e.m_envelope->setRate(scalar);
			else if (param == ENVELOPE_TIME)e->m_e.m_envelope->setTime(scalar);
			else if (param == ENVELOPE_TARGET)e->m_e.m_envelope->setTarget(scalar);
			else if (param == ENVELOPE_VALUE)e->m_e.m_envelope->setValue(scalar);
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Set Has No Effect
//--------------------------------------------------------------------
void STKEnvelopeSetHasNoEffect(STKEnvelope* e, bool hasnoeffect)
{
	if (hasnoeffect != e->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			e->m_tickCallback = [e](){return STKEnvelopeTickHasNoEffect(e); };
		}
		else
		{
			STKEnvelopeTerm(e);
			STKEnvelopeInit(e);
		}
	}
}

//--------------------------------------------------------------------
// STKEnvelope Node Tick()
//--------------------------------------------------------------------
StkFloat STKEnvelopeTickEnvelope(STKEnvelope* e)
{
	if (e->m_outidx == 0)
        return STKNodeUpdate((STKNode*)e, STKNodeTick(e->m_source) * e->m_e.m_envelope->tick())*e->m_volume;
	else
        return STKNodeUpdate((STKNode*)e, STKNodeTick(e->m_source) + e->m_e.m_envelope->lastOut());
}

StkFloat STKEnvelopeTickADSR(STKEnvelope* e)
{
	if (e->m_outidx == 0)
        return STKNodeUpdate((STKNode*)e, STKNodeTick(e->m_source) * e->m_e.m_adsr->tick())*e->m_volume;
	else
        return STKNodeUpdate((STKNode*)e, STKNodeTick(e->m_source) + e->m_e.m_adsr->lastOut());
}

StkFloat STKEnvelopeTickHasNoEffect(STKEnvelope* envelope)
{
	return 0;
}

StkFloat STKEnvelopeTick(STKEnvelope* e, unsigned int channel)
{
	return e->m_tickCallback();
}

