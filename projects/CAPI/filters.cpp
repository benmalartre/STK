#include "filters.h"
//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
STKFilter* STKFilterCreate(STKNode* source, STKFilterType type)
{
    STKFilter* filter = new STKFilter();
    filter->m_type = NODE_FILTER;
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

