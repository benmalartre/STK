#include "generators.h"
#include <cstdlib>

//--------------------------------------------------------------------
// STKGenerator Node Constructor
//--------------------------------------------------------------------
STKGenerator* STKGeneratorCreate(STKGeneratorType type, StkFloat frequency)
{
    STKGenerator* generator = new STKGenerator();
    generator->m_generator.m_asymp = NULL;
    generator->m_generator.m_noise = NULL;
    generator->m_generator.m_blit = NULL;
    generator->m_generator.m_blitsaw = NULL;
    generator->m_generator.m_blitsquare = NULL;
    generator->m_generator.m_sinewave = NULL;
    generator->m_generator.m_singwave = NULL;
    generator->m_generator.m_modulate = NULL;
    generator->m_generator.m_granulate = NULL;
    
	generator->m_noutput = 0;
	generator->m_volume = 1.0;
	generator->m_outidx = 0;
    generator->m_type = GENERATOR;
    
	generator->m_gentype = type;
	generator->m_frequency = frequency;
	STKGeneratorInit(generator);
    return generator;
}

//--------------------------------------------------------------------
// STKGenerator Node Destructor
//--------------------------------------------------------------------
void STKGeneratorDelete(STKGenerator* generator)
{
    STKGeneratorTerm(generator);
    delete generator;
}


//--------------------------------------------------------------------
// STKGenerator Node Init
//--------------------------------------------------------------------
void STKGeneratorInit(STKGenerator* generator)
{
    switch (generator->m_type)
    {
        case GENERATOR_ASYMP:
        {
            generator->m_generator.m_asymp = new Asymp();
            generator->m_generator.m_asymp->setValue(generator->m_frequency);
            generator->m_gentype = GENERATOR_ASYMP;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickAsymp(); };
            break;
        }
        case GENERATOR_NOISE:
        {
            generator->m_generator.m_noise = new Noise(m_frequency);
            generator->m_type = NOISE_GENERATOR;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickNoise(); };
            break;
        }
            
        case GENERATOR_ BLITSAW:
        {
            generator->m_generator.m_blitsaw = new BlitSaw(m_frequency);
            generator->m_type = BLITSAW_GENERATOR;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickBlitSaw(); };
            break;
        }
            
        case BLITSQUARE_GENERATOR:
        {
            generator->m_generator.m_blitsquare = new BlitSquare(m_frequency);
            generator->m_type = BLITSQUARE_GENERATOR;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickBlitSquare(); };
            break;
        }
            
        case BLIT_GENERATOR:
        {
            generator->m_generator.m_blit = new Blit(m_frequency);
            generator->m_type = BLIT_GENERATOR;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickBlit(); };
            break;
        }
            
        case SINEWAVE_GENERATOR:
        {
            generator->m_generator.m_sinewave = new SineWave();
            generator->m_sinewave->setFrequency(m_frequency);
            generator->m_type = SINEWAVE_GENERATOR;
            generator->m_tickCallback = [generator](){ return STKGeneratorTickSineWave(); };
            break;
        }
    }
}

//--------------------------------------------------------------------
// STKGenerator Node Terminate
//--------------------------------------------------------------------
void STKGeneratorTerm(STKGenerator* generator)
{
    switch(generator->m_type)
    {
        case ASYMP_GENERATOR:
        {
	        if (generator->m_generator.m_asymp)delete generator->m_generator.m_asymp;
            break;
        }
        case NOISE_GENERATOR:
        {
	        if (generator->m_generator.m_noise)delete generator->m_generator.m_noise;
            break;
        }
        case BLIT_GENERATOR:
        {
            if (generator->m_generator.m_blit)delete generator->m_generator.m_blit;
            break;
        }
        case BLITSAW_GENERATOR:
        {
            if (generator->m_generator.m_blitsaw)delete generator->m_generator.m_blitsaw;
            break;
        }
        case BLITSQUARE_GENERATOR:
        {
	        if (generator->m_generator.m_blitsquare)delete generator->m_generator.m_blitsquare;
            break;
        }
        case SINEWAVE_GENERATOR:
        {
            if (generator->m_generator.m_sinewave)delete generator->m_generator.m_sinewave;
            break;
        }
    }
}

//--------------------------------------------------------------------
// STKGenerator Node Reset
//--------------------------------------------------------------------
void STKGeneratorReset(STKGenerator* generator)
{
	generator->m_outidx = 0;
	switch (generator->m_type)
	{
        case GENERATOR_ASYMP:
        {
            break;
        }
            
        case GENERATOR_NOISE:
        {
            break;
        }

		case GENERATOR_BLITSAW:
		{
			generator->m_generator.m_blitsaw->reset();
			break;
		}

		case BLITSQUARE_GENERATOR:
		{
			generator->m_generator.m_blitsquare->reset();
			break;
		}
            
        case SINEWAVE_GENERATOR:
        {
            generator->m_generator.m_sinewave->reset();
            break;
        }
            
        case GENERATOR_BLIT:
        {
            generator->m_generator.m_blit->reset();
            break;
        }
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Type
//--------------------------------------------------------------------
void STKGeneratorSetType(STKGenerator* generator, STKGeneratorType type)
{
	if (generator->m_gentype != type)
	{
		STKGeneratorTerm(generator);
		generator->m_gentype = type;
		STKGeneratorInit(generator);
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Scalar
//--------------------------------------------------------------------
void STKGeneratorSetScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar)
{
	switch (generator->m_type){
		case GENERATOR_ASYMP:
		{
			if (param == T60)generator->m_generator.m_asymp->setT60(scalar);
			else if (param == TARGET)generator->m_generator.m_asymp->setTarget(scalar);
			else if (param == TAU)generator->m_generator.m_asymp->setTau(scalar);
			else if (param == TIME)generator->m_generator.m_asymp->setTime(scalar);
			else if (param == VALUE)generator->m_generator.m_asymp->setValue(scalar);
			break;
		}
		case GENERATOR_NOISE:
		{
			if(param == SEED)generator->m_generator.m_noise->setSeed((unsigned)scalar);
			break;
		}
		case GENERATOR_BLITSAW:
		{
			if (param == FREQUENCY)generator->m_generator.m_blitsaw->setFrequency(scalar);
			else if (param == HARMONICS)generator->m_generator.m_blitsaw->setHarmonics((unsigned)scalar);
			break;
		}
		case GENERATORBLITSQUARE:
		{
			if (param == FREQUENCY)generator->m_generator.m_blitsquare->setFrequency(scalar);
			else if (param == PHASE)generator->m_generator.m_blitsquare->setPhase(scalar);
			else if (param == HARMONICS)generator->m_generator.m_blitsquare->setHarmonics((unsigned)scalar);
			break;
		}
        case GENERATOR_BLIT:
        {
            if (param == FREQUENCY)generator->m_generator.m_blit->setFrequency(scalar);
            else if (param == PHASE)generator->m_generator.m_blit->setPhase(scalar);
            else if (param == HARMONICS)generator->m_generator.m_blit->setHarmonics((unsigned)scalar);
            break;
        }
		case GENERATOR_SINEWAVE:
		{
			if (param == FREQUENCY)generator->m_generator.m_sinewave->setFrequency(scalar);
			else if (param == PHASE)generator->m_generator.m_sinewave->addPhase(scalar);
			else if (param == PHASEOFFSET)generator->m_generator.m_sinewave->addPhase(scalar);
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKGeneratorSetHasNoEffect(STKGenerator* generator, bool hasnoeffect)
{
	if (hasnoeffect != generator->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			generator->m_tickCallback = [generator](){return STKGeneratorTickHasNoEffect(); };
		}
		else
		{
			STKGeneratorTerm(generator);
			STKGeneratorInit(generator);
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Tick()
//--------------------------------------------------------------------
StkFloat STKGeneratorTickAsymp(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_asymp->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_asymp->lastOut());
}
StkFloat STKGeneratorTickNoise(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_noise->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_noise->lastOut());
}

StkFloat STKGeneratorTickBlitSaw(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blitsaw->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blitsaw->lastOut());
}

StkFloat STKGeneratorTickBlitSquare(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blitsquare->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blitsquare->lastOut());
}

StkFloat STKGeneratorTickBlit(STKGenerator* g)
{
    if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blit->tick()*g->m_volume);
    else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_blit->lastOut());
}

StkFloat STKGeneratorTickSineWave(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_sinewave->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_generator.m_sinewave->lastOut());
}

StkFloat STKGeneratorTickHasNoEffect()
{
	return 0;
}
StkFloat STKGeneratorTick(STKGenerator* g, unsigned int channel)
{
	return g->m_tickCallback();
}

// ----------------------------------------------------------------------
//	SETTERS
// ----------------------------------------------------------------------
void STKSetGeneratorType(STKGenerator* generator, STKGeneratorType type)
{
	STKGeneratorSetType(generator, type);
}

void STKSetGeneratorScalar(STKGenerator* generator, STKGeneratorParam param, StkFloat scalar)
{
	STKGeneratorSetScalar(generator, param, scalar);
}

int STKGetGeneratorType(STKGenerator* node){
    return (int)STKGeneratorGetType(node);
}
