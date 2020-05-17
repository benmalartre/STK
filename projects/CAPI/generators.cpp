#include "generators.h"
#include <cstdlib>

//--------------------------------------------------------------------
// STKGenerator Node Constructor
//--------------------------------------------------------------------
STKGenerator* STKGeneratorCreate(STKGeneratorType type, StkFloat frequency)
{
    STKGenerator* g = new STKGenerator();
    g->m_type = STK_GENERATOR;
    
	  g->m_noutput = 0;
	  g->m_volume = 1.0;
	  g->m_outidx = 0;

	  g->m_gentype = type;
	  g->m_frequency = frequency;
	  STKGeneratorInit(g);
    return g;
}

//--------------------------------------------------------------------
// STKGenerator Node Destructor
//--------------------------------------------------------------------
void STKGeneratorDelete(STKGenerator* g)
{
    STKGeneratorTerm(g);
    delete g;
}


//--------------------------------------------------------------------
// STKGenerator Node Init
//--------------------------------------------------------------------
void STKGeneratorInit(STKGenerator* g)
{
    switch (g->m_gentype)
    {
        case GENERATOR_ASYMP:
        {
            g->m_g.m_asymp = new Asymp();
            g->m_g.m_asymp->setValue(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickAsymp(g); };
            break;
        }
        case GENERATOR_NOISE:
        {
            g->m_g.m_noise = new Noise(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickNoise(g); };
            break;
        }
            
        case GENERATOR_BLITSAW:
        {
            g->m_g.m_blitsaw = new BlitSaw(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickBlitSaw(g); };
            break;
        }
            
        case GENERATOR_BLITSQUARE:
        {
            g->m_g.m_blitsquare = new BlitSquare(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickBlitSquare(g); };
            break;
        }
            
        case GENERATOR_BLIT:
        {
            g->m_g.m_blit = new Blit(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickBlit(g); };
            break;
        }
            
        case GENERATOR_SINEWAVE:
        {
            g->m_g.m_sinewave = new SineWave();
            g->m_g.m_sinewave->setFrequency(g->m_frequency);
            g->m_tickCallback = [g](){ return STKGeneratorTickSineWave(g); };
            break;
        }
    }
}

//--------------------------------------------------------------------
// STKGenerator Node Terminate
//--------------------------------------------------------------------
void STKGeneratorTerm(STKGenerator* g)
{
    switch(g->m_gentype)
    {
        case GENERATOR_ASYMP:
        {
	        if (g->m_g.m_asymp)delete g->m_g.m_asymp;
            break;
        }
        case GENERATOR_NOISE:
        {
	        if (g->m_g.m_noise)delete g->m_g.m_noise;
            break;
        }
        case GENERATOR_BLIT:
        {
            if (g->m_g.m_blit)delete g->m_g.m_blit;
            break;
        }
        case GENERATOR_BLITSAW:
        {
            if (g->m_g.m_blitsaw)delete g->m_g.m_blitsaw;
            break;
        }
        case GENERATOR_BLITSQUARE:
        {
	        if (g->m_g.m_blitsquare)delete g->m_g.m_blitsquare;
            break;
        }
        case GENERATOR_SINEWAVE:
        {
            if (g->m_g.m_sinewave)delete g->m_g.m_sinewave;
            break;
        }
    }
}

//--------------------------------------------------------------------
// STKGenerator Node Reset
//--------------------------------------------------------------------
void STKGeneratorReset(STKGenerator* g)
{
	g->m_outidx = 0;
	switch (g->m_gentype)
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
			g->m_g.m_blitsaw->reset();
			break;
		}

		case GENERATOR_BLITSQUARE:
		{
			g->m_g.m_blitsquare->reset();
			break;
		}
            
        case GENERATOR_SINEWAVE:
        {
            g->m_g.m_sinewave->reset();
            break;
        }
            
        case GENERATOR_BLIT:
        {
            g->m_g.m_blit->reset();
            break;
        }
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Type
//--------------------------------------------------------------------
void STKGeneratorSetType(STKGenerator* g, STKGeneratorType type)
{
	if (g->m_gentype != type)
	{
		STKGeneratorTerm(g);
		g->m_gentype = type;
		STKGeneratorInit(g);
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Scalar
//--------------------------------------------------------------------
void STKGeneratorSetScalar(STKGenerator* g, STKGeneratorParam param, StkFloat scalar)
{
	switch (g->m_gentype){
		case GENERATOR_ASYMP:
		{
			if (param == GENERATOR_T60)g->m_g.m_asymp->setT60(scalar);
			else if (param == GENERATOR_TARGET)g->m_g.m_asymp->setTarget(scalar);
			else if (param == GENERATOR_TAU)g->m_g.m_asymp->setTau(scalar);
			else if (param == GENERATOR_TIME)g->m_g.m_asymp->setTime(scalar);
			else if (param == GENERATOR_VALUE)g->m_g.m_asymp->setValue(scalar);
			break;
		}
		case GENERATOR_NOISE:
		{
			if(param == GENERATOR_SEED)g->m_g.m_noise->setSeed((unsigned)scalar);
			break;
		}
		case GENERATOR_BLITSAW:
		{
			if (param == GENERATOR_FREQUENCY)g->m_g.m_blitsaw->setFrequency(scalar);
			else if (param == GENERATOR_HARMONICS)g->m_g.m_blitsaw->setHarmonics((unsigned)scalar);
			break;
		}
		case GENERATOR_BLITSQUARE:
		{
			if (param == GENERATOR_FREQUENCY)g->m_g.m_blitsquare->setFrequency(scalar);
			else if (param == GENERATOR_PHASE)g->m_g.m_blitsquare->setPhase(scalar);
			else if (param == GENERATOR_HARMONICS)g->m_g.m_blitsquare->setHarmonics((unsigned)scalar);
			break;
		}
        case GENERATOR_BLIT:
        {
            if (param == GENERATOR_FREQUENCY)g->m_g.m_blit->setFrequency(scalar);
            else if (param == GENERATOR_PHASE)g->m_g.m_blit->setPhase(scalar);
            else if (param == GENERATOR_HARMONICS)g->m_g.m_blit->setHarmonics((unsigned)scalar);
            break;
        }
		case GENERATOR_SINEWAVE:
		{
			if (param == GENERATOR_FREQUENCY)g->m_g.m_sinewave->setFrequency(scalar);
			else if (param == GENERATOR_PHASE)g->m_g.m_sinewave->addPhase(scalar);
			else if (param == GENERATOR_PHASEOFFSET)g->m_g.m_sinewave->addPhase(scalar);
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKGeneratorSetHasNoEffect(STKGenerator* g, bool hasnoeffect)
{
	if (hasnoeffect != g->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			g->m_tickCallback = [g](){return 0.f; };
		}
		else
		{
			STKGeneratorTerm(g);
			STKGeneratorInit(g);
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Tick()
//--------------------------------------------------------------------
StkFloat STKGeneratorTickAsymp(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_asymp->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_asymp->lastOut());
}
StkFloat STKGeneratorTickNoise(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_noise->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_noise->lastOut());
}

StkFloat STKGeneratorTickBlitSaw(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blitsaw->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blitsaw->lastOut());
}

StkFloat STKGeneratorTickBlitSquare(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blitsquare->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blitsquare->lastOut());
}

StkFloat STKGeneratorTickBlit(STKGenerator* g)
{
    if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blit->tick()*g->m_volume);
    else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_blit->lastOut());
}

StkFloat STKGeneratorTickSineWave(STKGenerator* g)
{
	if (g->m_outidx == 0)
        return STKNodeUpdate((STKNode*)g, g->m_g.m_sinewave->tick()*g->m_volume);
	else
        return STKNodeUpdate((STKNode*)g, g->m_g.m_sinewave->lastOut());
}

StkFloat STKGeneratorTickHasNoEffect()
{
	return 0;
}

StkFloat STKGeneratorTick(STKGenerator* g, unsigned int channel)
{
  return g->m_g.m_sinewave->tick();
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
