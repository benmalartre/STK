#include "effects.h"
#include "exports.h"

//--------------------------------------------------------------------
// STKEffect Node Constructor
//--------------------------------------------------------------------
STKEffect* STKEffectCreate(STKNode* source, STKEffectType type)
{
  STKEffect* fx = new STKEffect();
  fx->m_source = source;
  fx->m_volume = 1.0f;
  fx->m_fxtype = type;
  fx->m_type = STK_EFFECT;
  fx->m_noutput = 0;
  fx->m_outidx = 0;
  STKEffectInit(fx);
  return fx;
}

//--------------------------------------------------------------------
// STKEffect Node Constructor
//--------------------------------------------------------------------
void STKEffectDelete(STKEffect* fx)
{
	STKEffectTerm(fx);
    delete fx;
}

//--------------------------------------------------------------------
// STKEffect Node Init
//--------------------------------------------------------------------
void STKEffectInit(STKEffect* fx)
{
	switch (fx->m_fxtype)
	{
		case EFFECT_ENVELOPE:
		{
			fx->m_fx.m_envelope = new Envelope();
			fx->m_tickCallback = [fx](){ return STKEffectTickEnvelope(fx); };
			break;
		}
		case EFFECT_PRCREV:
		{
			fx->m_fx.m_prcrev = new PRCRev();
			fx->m_tickCallback = [fx](){ return STKEffectTickPRCRev(fx); };
			break;
		}
		case EFFECT_JCREV:
		{
			fx->m_fx.m_jcrev = new JCRev();
			fx->m_tickCallback = [fx](){ return STKEffectTickJCRev(fx); };
			break;
		}
		case EFFECT_NREV:
		{
			fx->m_fx.m_nrev = new NRev();
			fx->m_tickCallback = [fx](){ return STKEffectTickNRev(fx); };
			break;
		}
		case EFFECT_FREEVERB:
		{
			fx->m_fx.m_freeverb = new FreeVerb();
			fx->m_tickCallback = [fx](){ return STKEffectTickFreeVerb(fx); };
			break;
		}
		case EFFECT_ECHO:
		{
			fx->m_fx.m_echo = new Echo();
			fx->m_tickCallback = [fx](){ return STKEffectTickEcho(fx); };
			break;
		}
		case EFFECT_PITSHIFT:
		{
			fx->m_fx.m_pitshift = new PitShift();
			fx->m_tickCallback = [fx](){ return STKEffectTickPitShift(fx); };
			break;
		}
		case EFFECT_LENTPITSHIFT:
		{
			fx->m_fx.m_lentpitshift = new LentPitShift();
			fx->m_tickCallback = [fx](){ return STKEffectTickLentPitShift(fx); };
			break;
		}
		case EFFECT_CHORUS:
		{
			fx->m_fx.m_chorus = new Chorus();
			fx->m_tickCallback = [fx](){ return STKEffectTickChorus(fx); };
			break;
		}
		case EFFECT_MOOG:
		{
			fx->m_fx.m_moog = new Moog();
			fx->m_tickCallback = [fx](){ return STKEffectTickMoog(fx); };
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKEffect Node Term
//--------------------------------------------------------------------
void STKEffectTerm(STKEffect* fx)
{
}

//--------------------------------------------------------------------
// STKEffect Node Tick()
//--------------------------------------------------------------------
StkFloat STKEffectTickEnvelope(STKEffect* fx)
{
	if (fx->m_outidx == 0)
        return STKNodeUpdate((STKNode*)fx, fx->m_fx.m_envelope->tick()) *
            STKNodeTick(fx->m_source) *
            fx->m_volume;
    
	else
        return STKNodeUpdate((STKNode*) fx, fx->m_fx.m_envelope->lastOut()) *
            STKNodeTick(fx->m_source) *
            fx->m_volume;
}

StkFloat STKEffectTickPRCRev(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_prcrev->tick(m_source->tick()))* m_volume;
	else return update(m_prcrev->lastOut())* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickJCRev(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_jcrev->tick(m_source->tick()))* m_volume;
	else return update(m_jcrev->lastOut())* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickNRev(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_nrev->tick(m_source->tick()))* m_volume;
	else return update(m_nrev->lastOut())* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickFreeVerb(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_freeverb->tick(m_source->tick()))* m_volume;
	else return update(m_freeverb->lastOut())* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickEcho(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_prcrev->tick(m_source->tick()))* m_volume;
	else return update(m_prcrev->lastOut())* m_volume;
	return m_echo->tick(m_source->tick())* m_volume;
    */
    return 0;
}

StkFloat STKEffectTickPitShift(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_pitshift->tick(m_source->tick()))* m_volume;
	else return update(m_pitshift->lastOut())* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickLentPitShift(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_lentpitshift->tick(m_source->tick()))* m_volume;
	else return update(m_lentpitshift->tick(m_source->tick()))* m_volume;
     */
    return 0;
}

StkFloat STKEffectTickChorus(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_chorus->tick(m_source->tick()))* m_volume;
	else return update(m_chorus->lastOut())* m_volume;
     */
    return 0;

}

StkFloat STKEffectTickMoog(STKEffect* fx)
{
    /*
	if (m_outidx == 0) return update(m_moog->tick(m_source->tick()))* m_volume;
	else return update(m_moog->lastOut())* m_volume;
     */
    return 0;

}

StkFloat STKEffectTickHasNoEffect(STKEffect* fx)
{
	return STKNodeTick(fx->m_source);
}

StkFloat STKEffectTick(STKEffect* fx, unsigned int channel)
{
	return fx->m_tickCallback();
}

//--------------------------------------------------------------------
// STKEffect Node Change Type
//--------------------------------------------------------------------
STKEffectType STKEffectGetType(STKEffect* fx)
{ 
	return fx->m_fxtype; 
};

void STKEffectSetType(STKEffect* fx, STKEffectType type)
{
	if (type != fx->m_fxtype){
		STKEffectTerm(fx);
		fx->m_fxtype = type;
		STKEffectInit(fx);
	}
}

//--------------------------------------------------------------------
// STKEffect Node Change Scalar
//--------------------------------------------------------------------
void STKEffectSetScalar( STKEffect* fx, StkFloat scalar, STKEffectParam param)
{
	switch (fx->m_fxtype){
		case EFFECT_ENVELOPE:
		{
			if (param == EFFECT_RATE)
			{
				if (scalar < 0)scalar = 0;
				fx->m_fx.m_envelope->setRate(scalar);
			}
			else if (param == EFFECT_TIME)fx->m_fx.m_envelope->setTime(scalar);
			else if (param == EFFECT_TARGET)fx->m_fx.m_envelope->setTarget(scalar);
			else if (param == EFFECT_VALUE)fx->m_fx.m_envelope->setValue(scalar);
			break;
		}
		case EFFECT_PRCREV:
		{
			if (param == EFFECT_T60)fx->m_fx.m_prcrev->setT60(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_prcrev->setEffectMix(scalar);
            break;
		}
		case EFFECT_JCREV:
		{
			if (param == EFFECT_T60)fx->m_fx.m_jcrev->setT60(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_jcrev->setEffectMix(scalar);
            break;
		}
		case EFFECT_NREV:
		{
			if (param == EFFECT_T60)fx->m_fx.m_nrev->setT60(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_nrev->setEffectMix(scalar);
            break;
		}
		case EFFECT_FREEVERB:
		{
			if (param == EFFECT_MIX)fx->m_fx.m_freeverb->setEffectMix(scalar);
			else if (param == EFFECT_ROOMSIZE)fx->m_fx.m_freeverb->setRoomSize(scalar);
			else if (param == EFFECT_DAMPING)fx->m_fx.m_freeverb->setDamping(scalar);
			else if (param == EFFECT_WIDTH)fx->m_fx.m_freeverb->setWidth(scalar);
			else if (param == EFFECT_MODE)fx->m_fx.m_freeverb->setMode((bool)scalar);
            break;
		}
		case EFFECT_ECHO:
		{
			if (param == EFFECT_DELAY)fx->m_fx.m_echo->setDelay(scalar);
			else if (param == EFFECT_MAXIMUMDELAY)fx->m_fx.m_echo->setMaximumDelay(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_echo->setEffectMix(scalar);
            break;
		}
		case EFFECT_PITSHIFT:
		{
			if (param == EFFECT_SHIFT)fx->m_fx.m_pitshift->setShift(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_pitshift->setEffectMix(scalar);
            break;
		}
		case EFFECT_LENTPITSHIFT:
		{
			if (param == EFFECT_SHIFT)fx->m_fx.m_lentpitshift->setShift(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_lentpitshift->setEffectMix(scalar);
            break;
		}
		case EFFECT_CHORUS:
		{
			if (param == EFFECT_MODDEPTH)fx->m_fx.m_chorus->setModDepth(scalar);
			else if (param == EFFECT_MODFREQUENCY)fx->m_fx.m_chorus->setModFrequency(scalar);
			else if (param == EFFECT_MIX)fx->m_fx.m_chorus->setEffectMix(scalar);
            break;
		}
        case EFFECT_MOOG:
        {
            break;
        }
	}
}

//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKEffectSetHasNoEffect(STKEffect* fx, bool hasnoeffect)
{
	if (hasnoeffect != fx->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			fx->m_tickCallback = [fx](){return STKEffectTickHasNoEffect(fx); };
		}
		else
		{
			STKEffectTerm(fx);
			STKEffectInit(fx);
		}
	}
}

