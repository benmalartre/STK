#include "generators.h"

//--------------------------------------------------------------------
// STKGenerator Node Constructor
//--------------------------------------------------------------------
STKGenerator::STKGenerator(Type type, float frequency)
{
	m_noutput = 0;
	m_outidx = 0;
	m_type = type;
	m_frequency = frequency;
	init();
}

//--------------------------------------------------------------------
// STKGenerator Node Terminate
//--------------------------------------------------------------------
void STKGenerator::term()
{
	/*
	if (m_adsr)delete m_adsr;
	if (m_envelope)delete m_envelope;
	if (m_granulate)delete m_granulate;
	if (m_modulate)delete m_modulate;
	*/
	if (m_asymp)delete m_asymp;
	if (m_noise)delete m_noise;
	if (m_blit)delete m_blit;
	if (m_blitsaw)delete m_blitsaw;
	if (m_blitsquare)delete m_blitsquare;
	//if (m_singwave)delete m_singwave;
	if (m_sinewave)delete m_sinewave;
}

//--------------------------------------------------------------------
// STKGenerator Node Init
//--------------------------------------------------------------------
void STKGenerator::init()
{
	switch (m_type)
	{
		/*
	case ADSR_GENERATOR:
	{
		m_adsr = new ADSR();
		m_adsr->setValue(m_frequency);
		m_type = ADSR_GENERATOR;
		m_tickCallback = [this](){ return this->ADSRTickCallback(); };
		break;
	}

	case ENVELOPE_GENERATOR:
	{
		m_envelope = new Envelope();
		m_type = ENVELOPE_GENERATOR;
		m_tickCallback = [this](){ return this->EnvelopeTickCallback(); };
		break;
	}

	case GRANULATE_GENERATOR:
	{
		m_granulate = new Granulate();
		m_type = GRANULATE_GENERATOR;
		m_tickCallback = [this](){ return this->GranulateTickCallback(); };
		break;
	}

	case MODULATE_GENERATOR:
	{
		m_modulate = new Modulate();
		m_type = MODULATE_GENERATOR;
		m_tickCallback = [this](){ return this->ModulateTickCallback(); };
		break;
	}
	*/
	case ASYMP_GENERATOR:
	{
		m_asymp = new Asymp();
		m_asymp->setValue(m_frequency);
		m_type = ASYMP_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickAsymp(); };
		break;
	}
	case NOISE_GENERATOR:
	{
		m_noise = new Noise(m_frequency);
		m_type = NOISE_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickNoise(); };
		break;
	}

	case BLITSAW_GENERATOR:
	{
		m_blitsaw = new BlitSaw(m_frequency);
		m_type = BLITSAW_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickBlitSaw(); };
		break;
	}

	case BLITSQUARE_GENERATOR:
	{
		m_blitsquare = new BlitSquare(m_frequency);
		m_type = BLITSQUARE_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickBlitSquare(); };
		break;
	}

	case BLIT_GENERATOR:
	{
		m_blit = new Blit(m_frequency);
		m_type = BLIT_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickBlit(); };
		break;
	}

	case SINEWAVE_GENERATOR:
	{
		m_sinewave = new SineWave();
		m_sinewave->setFrequency(m_frequency);
		m_type = SINEWAVE_GENERATOR;
		m_tickCallback = [this](){ return this->GeneratorTickSineWave(); };
		break;
	}
		/*
	case SINGWAVE_GENERATOR:
	{
		m_singwave = new SingWave();
		m_singwave->setFrequency(frequency);
		m_type = STKGeneratorType::SINGWAVE_GENERATOR;
		m_tickCallback = [this](){ return this->SingWaveTickCallback(); };
		break;
	}
	*/
	}
}


//--------------------------------------------------------------------
// STKGenerator Node Destructor
//--------------------------------------------------------------------
STKGenerator::~STKGenerator()
{
	term();
}

//--------------------------------------------------------------------
// STKGenerator Node Reset
//--------------------------------------------------------------------
void STKGenerator::reset()
{
	m_outidx = 0;
	switch (m_type)
	{
		case SINEWAVE_GENERATOR:
		{
			m_sinewave->reset();
			break;
		}

		case BLIT_GENERATOR:
		{
			m_blit->reset();
			break;
		}

		case BLITSAW_GENERATOR:
		{
			m_blitsaw->reset();
			break;
		}

		case BLITSQUARE_GENERATOR:
		{
			m_blitsquare->reset();
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Type
//--------------------------------------------------------------------
void STKGenerator::setType(Type type)
{
	if (type != m_type)
	{
		term();
		m_type = type;
		init();
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Scalar
//--------------------------------------------------------------------
void STKGenerator::setScalar(Param param, StkFloat scalar)
{
	switch (m_type){
		case ASYMP_GENERATOR:
		{
			if (param == T60)m_asymp->setT60(scalar);
			else if (param == TARGET)m_asymp->setTarget(scalar);
			else if (param == TAU)m_asymp->setTau(scalar);
			else if (param == TIME)m_asymp->setTime(scalar);
			else if (param == VALUE)m_asymp->setValue(scalar);
		}
		case NOISE_GENERATOR:
		{
			if(param == SEED)m_noise->setSeed((unsigned)scalar);
		}
		case BLIT_GENERATOR:
		{
			if (param == FREQUENCY)m_blit->setFrequency(scalar);
			else if (param == PHASE)m_blit->setPhase(scalar);
			else if (param == HARMONICS)m_blit->setHarmonics((unsigned)scalar);
		}
		case BLITSAW_GENERATOR:
		{
			if (param == FREQUENCY)m_blitsaw->setFrequency(scalar);
			else if (param == HARMONICS)m_blitsaw->setHarmonics((unsigned)scalar);
		}
		case BLITSQUARE_GENERATOR:
		{
			if (param == FREQUENCY)m_blitsquare->setFrequency(scalar);
			else if (param == PHASE)m_blitsquare->setPhase(scalar);
			else if (param == HARMONICS)m_blitsquare->setHarmonics((unsigned)scalar);
		}
		case SINEWAVE_GENERATOR:
		{
			if (param == FREQUENCY)m_sinewave->setFrequency(scalar);
			else if (param == PHASE)m_sinewave->addPhase(scalar);
			else if (param == PHASEOFFSET)m_sinewave->addPhase(scalar);
		}
		/*
		case SINGWAVE_GENERATOR:
		{
			if (param == FREQUENCY)m_singwave->setFrequency(scalar);
			else if (param == PHASE)m_singwave->se(scalar);
			else if (param == PHASEOFFSET)m_singwave->addPhase(scalar);
		}
		*/
	}
	
}

//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKGenerator::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->GeneratorTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Tick()
//--------------------------------------------------------------------
/*
StkFloat STKGenerator::EnvelopeTickCallback()
{
	return m_envelope->tick();
}
StkFloat STKGenerator::ADSRTickCallback()
{
	return m_adsr->tick();
}

StkFloat STKGenerator::ModulateTickCallback()
{
	return m_modulate->tick();
}

StkFloat STKGenerator::GranulateTickCallback()
{
	return m_granulate->tick();
}
*/
StkFloat STKGenerator::GeneratorTickAsymp()
{
	if (m_outidx == 0)return update(m_asymp->tick());
	else return update(m_asymp->lastOut());
}
StkFloat STKGenerator::GeneratorTickNoise()
{
	if (m_outidx == 0)return update(m_noise->tick());
	else return update(m_noise->lastOut());
}

StkFloat STKGenerator::GeneratorTickBlit()
{
	if (m_outidx == 0)return update(m_blit->tick());
	else return update(m_blit->lastOut());
}

StkFloat STKGenerator::GeneratorTickBlitSaw()
{
	if (m_outidx == 0)return update(m_blitsaw->tick());
	else return update(m_blitsaw->lastOut());
}

StkFloat STKGenerator::GeneratorTickBlitSquare()
{
	if (m_outidx == 0)return update(m_blitsquare->tick());
	else return update(m_blitsquare->lastOut());
}

StkFloat STKGenerator::GeneratorTickSineWave()
{
	if (m_outidx == 0)return update(m_sinewave->tick());
	else return update(m_sinewave->lastOut());
}

/*
StkFloat STKGenerator::SingWaveTickCallback()
{
	return m_singwave->tick();
}
*/
StkFloat STKGenerator::GeneratorTickHasNoEffect()
{
	return 0;
}
StkFloat STKGenerator::tick(unsigned int channel)
{
	return m_tickCallback();
}

// ----------------------------------------------------------------------
//	SETTERS
// ----------------------------------------------------------------------
void STKSetGeneratorType(STKGenerator* generator, STKGenerator::Type type)
{
	generator->setType(type);
}

void STKSetGeneratorScalar(STKGenerator* generator, STKGenerator::Param param, StkFloat scalar)
{
	generator->setScalar(param, scalar);
}