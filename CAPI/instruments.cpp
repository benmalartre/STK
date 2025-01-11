#include "instruments.h"
//--------------------------------------------------------------------
// STKFilter Node Constructor
//--------------------------------------------------------------------
STKInstruments* STKInstrumentCreate(STKNode* source, STKInstrumentType type)
{
  STKInstruments* instruments = new STKInstruments();
  instruments->m_type = STK_INSTRUMENT;
  instruments->m_source = source;
  instruments->m_volume = 1.0f;
  instruments->m_instrumentType = INSTRUMENT_CLARINET;
  STKInstrumentInit(instruments);
}

//--------------------------------------------------------------------
// STKInstruments Node Destructor
//--------------------------------------------------------------------
void STKInstrumentDelete(STKInstruments* instrument)
{
  STKInstrumentTerm(instrument);
  delete instrument;
}
//--------------------------------------------------------------------
// STKInstruments Node Init
//--------------------------------------------------------------------
void STKInstrumentInit(STKInstruments* instrument)
{
  switch (instrument->m_instrumentType)
  {
    case INSTRUMENT_CLARINET:
    {
      instrument->m_i->m_clarinet = new Clarinet();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickClarinet(instrument); };
      break;
    }
    case INSTRUMENT_BLOWHOLE:
    {
      instrument->m_i->m_blowhole = new BlowHole(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickBlowHole(instrument); };
      break;
    }
    case INSTRUMENT_SAXOFONY:
    {
      instrument->m_i->m_saxofony = new Saxofony(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickSaxophony(instrument); };
      break;
    }
    case INSTRUMENT_FLUTE:
    {
      instrument->m_i->m_flute = new Flute(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickFlute(instrument); };
      break;
    }
    case INSTRUMENT_BRASS:
    {
      instrument->m_i->m_brass = new Brass(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickBrass(instrument); };
      break;
    }
    case INSTRUMENT_BLOWBOTL:
    {
      instrument->m_i->m_blowbtl = new BlowBotl();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickBlowBotl(instrument); };
      break;
    }
    case INSTRUMENT_BOWED:
    {
      instrument->m_i->m_bowed = new Bowed(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickBowed(instrument); };
      break;
    }
    case INSTRUMENT_PLUCKED:
    {
      instrument->m_i->m_plucked = new Plucked(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickPlucked(instrument); };
      break;
    }
    case INSTRUMENT_STIFKARP:
    {
      instrument->m_i->m_stifkarp = new StifKarp(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickStifKarp(instrument); };
      break;
    }
    case INSTRUMENT_SITAR:
    {
      instrument->m_i->m_sitar = new Sitar(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickSitar(instrument); };
      break;
    }
    case INSTRUMENT_MANDOLIN:
    {
      instrument->m_i->m_mandolin = new Mandolin(110);
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickMandolin(instrument); };
      break;
    }
    case INSTRUMENT_RHODEY:
    {
      instrument->m_i->m_rhodey = new Rhodey();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickRhodey(instrument); };
      break;
    }
    case INSTRUMENT_WURLEY:
    {
      instrument->m_i->m_wurley = new Wurley();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickWurley(instrument); };
      break;
    }
    case INSTRUMENT_TUBEBELL:
    {
      instrument->m_i->m_tubebell = new TubeBell();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickTubeBell(instrument); };
      break;
    }
    case INSTRUMENT_HEAVYMETL:
    {
      instrument->m_i->m_hevymtl = new HevyMetl();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickHeavyMetl(instrument); };
      break;
    }
    case INSTRUMENT_PERCFLUT:
    {
      instrument->m_i->m_percflut = new PercFlut();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickPercFlut(instrument); };
      break;
    }
    case INSTRUMENT_BEETHREE:
    {
      instrument->m_i->m_beethree = new BeeThree();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickBeeThree(instrument); };
      break;
    }
    case INSTRUMENT_MOOG:
    {
      instrument->m_i->m_moog = new Moog();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickMoog(instrument); };
      break;
    }
    case INSTRUMENT_FMVOICES:
    {
      instrument->m_i->m_fmvoices = new FMVoices();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickFMVoices(instrument); };
      break;
    }
    case INSTRUMENT_VOICFORM:
    {
      instrument->m_i->m_voicform = new VoicForm();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickVoicForm(instrument); };
      break;
    }
    case INSTRUMENT_RESONATE:
    {
      instrument->m_i->m_resonate = new Resonate();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickResonate(instrument); };
      break;
    }
    case INSTRUMENT_DRUMMER:
    {
      instrument->m_i->m_drummer = new Drummer();
      instrument->m_tickCallback = [instrument]() { return STKInstrumentTickDrummer(instrument); };
      break;
    }
    /*
      INSTRUMENT_BANDEDWG,
      INSTRUMENT_SHAKERS,
      INSTRUMENT_MODALBAR,
      INSTRUMENT_MESH2D,
      INSTRUMENT_WHISTLE
    */
    default:
    {
      return;
    }
  }
}


//--------------------------------------------------------------------
// STKInstruments Node Term
//--------------------------------------------------------------------
void STKInstrumentTerm(STKInstruments* instrument)
{
}


//--------------------------------------------------------------------
// STKInstruments Node Tick()
//--------------------------------------------------------------------
StkFloat STKInstrumentTickClarinet(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_clarinet->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_clarinet->lastOut());
}

inline StkFloat STKInstrumentTickBlowHole(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_blowhole->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_blowhole->lastOut());
}

inline StkFloat STKInstrumentTickSaxophony(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_saxofony->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_saxofony->lastOut());
}

inline StkFloat STKInstrumentTickFlute(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_flute->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_flute->lastOut());
}

inline StkFloat STKInstrumentTickBrass(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_brass->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_brass->lastOut());
}


inline StkFloat STKInstrumentTickBlowBotl(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_blowbtl->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_blowbtl->lastOut());
}

inline StkFloat STKInstrumentTickBowed(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_bowed->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_bowed->lastOut());
}

inline StkFloat STKInstrumentTickPlucked(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_plucked->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_plucked->lastOut());
}

inline StkFloat STKInstrumentTickStifKarp(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_stifkarp->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_stifkarp->lastOut());
}

inline StkFloat STKInstrumentTickSitar(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_sitar->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_sitar->lastOut());
}

inline StkFloat STKInstrumentTickMandolin(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_mandolin->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_mandolin->lastOut());
}

inline StkFloat STKInstrumentTickRhodey(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_rhodey->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_rhodey->lastOut());
}

inline StkFloat STKInstrumentTickWurley(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_wurley->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_wurley->lastOut());
}

inline StkFloat STKInstrumentTickTubeBell(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_tubebell->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_tubebell->lastOut());
}

inline StkFloat STKInstrumentTickHeavyMetl(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_hevymtl->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_hevymtl->lastOut());
}

inline StkFloat STKInstrumentTickPercFlut(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_percflut->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_percflut->lastOut());
}

inline StkFloat STKInstrumentTickBeeThree(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_beethree->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_beethree->lastOut());
}

inline StkFloat STKInstrumentTickMoog(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_moog->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_moog->lastOut());
}

inline StkFloat STKInstrumentTickFMVoices(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_fmvoices->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_fmvoices->lastOut());
}

inline StkFloat STKInstrumentTickVoicForm(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_voicform->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_voicform->lastOut());
}

inline StkFloat STKInstrumentTickResonate(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_resonate->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_resonate->lastOut());
}

inline StkFloat STKInstrumentTickDrummer(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_drummer->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_drummer->lastOut());
}

inline StkFloat STKInstrumentTickBandedWG(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_bandedwg->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_bandedwg->lastOut());
}


inline StkFloat STKInstrumentTickShakers(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_shakers->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_shakers->lastOut());
}

inline StkFloat STKInstrumentTickModalBar(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_modelbar->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_modelbar->lastOut());
}

inline StkFloat STKInstrumentTickMesh2D(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_mesh2D->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_mesh2D->lastOut());
}

inline StkFloat STKInstrumentTickWhistle(STKInstruments* i)
{
  if (i->m_outidx == 0)
    return STKNodeUpdate((STKNode*)i, i->m_i->m_whistle->tick() * i->m_volume);
  else
    return STKNodeUpdate((STKNode*)i, i->m_i->m_whistle->lastOut());
}

inline StkFloat STKInstrumentTickHasNoEffect(STKInstruments* i)
{
  return 0.f;
}


StkFloat STKInstrumentTick(STKInstruments* i, unsigned int channel)
{
  return i->m_tickCallback();
}
//--------------------------------------------------------------------
// STKEffect Node Change Type
//--------------------------------------------------------------------
void STKInstrumentsSetType(STKInstruments* instrument, STKInstrumentType type)
{
  /*
	if (type != m_type){
		term();
		m_type = type;
		init();
	}
  */
}

//--------------------------------------------------------------------
// STKEffect Node Change Scalar
//--------------------------------------------------------------------
void STKInstrumentsSetScalar(StkFloat scalar, STKInstrumentParam param)
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
// STKInstruments Set Has No Effect
//--------------------------------------------------------------------
void STKInstrumentsSetHasNoEffect(STKInstruments* instrument, bool hasnoeffect)
{
	if (hasnoeffect != instrument->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
      instrument->m_tickCallback = [instrument]() {return STKInstrumentTickHasNoEffect(instrument); };
		}
		else
		{
			STKInstrumentTerm(instrument);
			STKInstrumentInit(instrument);
		}
	}
}

// ----------------------------------------------------------------------
//	STK INSTRUMENTS NODE SETTER
// ----------------------------------------------------------------------
void STKSetInstrumentType(STKInstruments* instrument, STKInstrumentType type)
{
  STKInstrumentTerm(instrument);
  instrument->m_instrumentType = type;
  STKInstrumentInit(instrument);
}

void STKSetInstrumentScalar(STKInstruments* instrument, STKInstrumentParam param, StkFloat scalar)
{
  //filter->setScalar(scalar, param);
}
