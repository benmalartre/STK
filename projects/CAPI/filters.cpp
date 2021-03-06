#include "filters.h"
#include "exports.h"
//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
STKFilter* STKFilterCreate(STKNode* source, STKFilterType type)
{
  STKFilter* filter = new STKFilter();
  filter->m_type = STK_FILTER;
  filter->m_source = source;
  filter->m_volume = 1.0f;
  filter->m_ftype = type;
  filter->m_noutput = 0;
  filter->m_outidx = 0;
  STKFilterInit(filter);
  return filter;
}

//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
void STKFilterDelete(STKFilter* filter)
{
	  STKFilterTerm(filter);
    delete filter;
}

//--------------------------------------------------------------------
// STKFilter Node Init
//--------------------------------------------------------------------
void STKFilterInit(STKFilter* filter)
{
	switch (filter->m_type)
	{
        case FILTER_ONEZERO:
		{
			filter->m_filter.m_onezero = new OneZero();
			filter->m_tickCallback = [filter](){ return STKFilterTickOneZero(filter); };
			break;
		}
		case FILTER_POLEZERO:
		{
			filter->m_filter.m_polezero = new PoleZero();
			filter->m_tickCallback = [filter](){ return STKFilterTickPoleZero(filter); };
			break;
		}
		case FILTER_TWOZERO:
		{
			filter->m_filter.m_twozero = new TwoZero();
			filter->m_tickCallback = [filter](){ return STKFilterTickTwoZero(filter); };
			break;
		}
		case FILTER_TWOPOLE:
		{
			filter->m_filter.m_twopole = new TwoPole();
			filter->m_tickCallback = [filter](){ return STKFilterTickTwoPole(filter); };
			break;
		}
		case FILTER_BIQUAD:
		{
			filter->m_filter.m_biquad = new BiQuad();
			filter->m_tickCallback = [filter](){ return STKFilterTickBiQuad(filter); };
			break;
		}
		case FILTER_FORMSWEP:
		{
			filter->m_filter.m_formswep = new FormSwep();
			filter->m_tickCallback = [filter](){ return STKFilterTickFormSwep(filter); };
			break;
		}
		case FILTER_DELAY:
		{
			filter->m_filter.m_delay = new Delay();
			filter->m_tickCallback = [filter](){ return STKFilterTickDelay(filter); };
			break;
		}
		case FILTER_DELAYA:
		{
			filter->m_filter.m_delaya = new DelayA();
			filter->m_tickCallback = [filter](){ return STKFilterTickDelayA(filter); };
			break;
		}
		case FILTER_DELAYL:
		{
			filter->m_filter.m_delayl = new DelayL();
			filter->m_tickCallback = [filter](){ return STKFilterTickDelayL(filter); };
			break;
		}
		default:
		{
			return;
		}
	}
}


//--------------------------------------------------------------------
// STKFilter Node Term
//--------------------------------------------------------------------
void STKFilterTerm(STKFilter* filter)
{
}


//--------------------------------------------------------------------
// STKFilter Node Tick()
//--------------------------------------------------------------------
StkFloat STKFilterTickOneZero(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_onezero->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_onezero->lastOut());
}

StkFloat STKFilterTickOnePole(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_onepole->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_onepole->lastOut());
}

StkFloat STKFilterTickPoleZero(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_polezero->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_polezero->lastOut());
}

StkFloat STKFilterTickTwoZero(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_twozero->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_twozero->lastOut());
}

StkFloat STKFilterTickTwoPole(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_twopole->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_twopole->lastOut());
}

StkFloat STKFilterTickBiQuad(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_biquad->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_biquad->lastOut());
}

StkFloat STKFilterTickFormSwep(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_formswep->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_formswep->lastOut());
}

StkFloat STKFilterTickDelay(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delay->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delay->lastOut());
}

StkFloat STKFilterTickDelayA(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delaya->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delaya->lastOut());
}

StkFloat STKFilterTickDelayL(STKFilter* f)
{
	if (f->m_outidx == 0)
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delayl->tick(STKNodeTick(f->m_source)))*f->m_volume;
	else
        return STKNodeUpdate((STKNode*)f, f->m_filter.m_delayl->lastOut());
}

StkFloat STKFilterTickHasNoEffect(STKFilter* f)
{
	return STKNodeTick(f->m_source);
}

StkFloat STKFilterTick(STKFilter* f, unsigned int channel)
{
	return f->m_tickCallback();
}

//--------------------------------------------------------------------
// STKEffect Node Change Type
//--------------------------------------------------------------------
void STKFilterSetType(STKFilter* filter, STKFilterType type)
{
    
}

//--------------------------------------------------------------------
// STKEffect Node Change Scalar
//--------------------------------------------------------------------
void STKFilterSetScalar(STKFilter* filter, StkFloat scalar, STKFilterParam param)
{
	/*
	switch (filter->m_type){
		case FILTER_ENVELOPE:
		{
			if (param == FILTER_RATE)
			{
				if (scalar < 0)scalar = 0;
				filter->m_envelope->setRate(scalar);
			}
			else if (param == FILTER_TIME)filter->m_envelope->setTime(scalar);
			else if (param == FILTER_TARGET)filter->m_envelope->setTarget(scalar);
			else if (param == FILTER_VALUE)filter->m_envelope->setValue(scalar);
			break;
		}
		case FILTER_PRCREV:
		{
			if (param == FILTER_T60)filter->_prcrev->setT60(scalar);
			else if (param == FILTER_MIX)filter->m_prcrev->setEffectMix(scalar);
		}
		case FILTER_JCREV:
		{
			if (param == FILTER_T60)filter->m_jcrev->setT60(scalar);
			else if (param == FILTER_MIX)filter->m_jcrev->setEffectMix(scalar);
		}
		case FILTER_NREV:
		{
			if (param == FILTER_T60)filter->m_nrev->setT60(scalar);
			else if (param == FILTER_MIX)filter->m_nrev->setEffectMix(scalar);
		}
		case FILTER_FREEVERB:
		{
			if (param == FILTER_MIX)filter->m_freeverb->setEffectMix(scalar);
			else if (param == FILTER_ROOMSIZE)filter->m_freeverb->setRoomSize(scalar);
			else if (param == FILTER_DAMPING)filter->m_freeverb->setDamping(scalar);
			else if (param == FILTER_WIDTH)filter->m_freeverb->setWidth(scalar);
			else if (param == FILTER_MODE)filter->m_freeverb->setMode((bool)scalar);
		}
		case FILTER_ECHO:
		{
			if (param == FILTER_DELAY)filter->m_echo->setDelay(scalar);
			else if (param == FILTER_MAXIMUMDELAY)filter->m_echo->setMaximumDelay(scalar);
			else if (param == FILTER_MIX)filter->m_echo->setEffectMix(scalar);
		}
		case FILTER_PITSHIFT:
		{
			if (param == FILTER_SHIFT)filter->m_pitshift->setShift(scalar);
			else if (param == FILTER_MIX)filter->m_pitshift->setEffectMix(scalar);
		}
		case FILTER_LENTPITSHIFT:
		{
			if (param == FILTER_SHIFT)filter->m_lentpitshift->setShift(scalar);
			else if (param == FILTER_MIX)filter->m_lentpitshift->setEffectMix(scalar);
		}
		case FILTER_CHORUS:
		{
			if (param == FILTER_MODDEPTH)filter->m_chorus->setModDepth(scalar);
			else if (param == FILTER_MODFREQUENCY)filter->m_chorus->setModFrequency(scalar);
			else if (param == FILTER_MIX)filter->m_chorus->setEffectMix(scalar);
		}
	}
	*/
}

//--------------------------------------------------------------------
// STKFilter Set Has No Effect
//--------------------------------------------------------------------
void STKFilterSetHasNoEffect(STKFilter* filter, bool hasnoeffect)
{
	if (hasnoeffect != filter->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			filter->m_tickCallback = [filter](){return STKFilterTickHasNoEffect(filter); };
		}
		else
		{
			STKFilterTerm(filter);
			STKFilterInit(filter);
		}
	}
}

EXPORT STKFilterType STKFilterGetType(STKFilter* f){ return f->m_ftype; };

