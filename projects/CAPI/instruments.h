#pragma once

#include "common.h"
#include "nodes.h"
#include "Clarinet.h"// Pretty good physical model of the clarinet
#include "BlowHole.h"// A clarinet physical model with one tonehole and one register vent
#include "Saxofony.h"// A psuedo-conical bore reed instrument that sometimes sounds like a saxophone
#include "Flute.h"// Pretty good physical model of the flute
#include "Brass.h"// Not so bad physical model of a brass instrument
#include "BlowBotl.h"// A basic helmholtz resonator and air jet model
#include "Bowed.h"// Not hideous physical model of a bowed string instrument
#include "Plucked.h"// Yer basic plucked string physical model
#include "StifKarp.h"// A simple plucked, stiff string physical model
#include "Sitar.h"// A simple sitar/plucked string physical model
#include "Mandolin.h"// Two-string mandolin physical model
#include "Rhodey.h"// Rhodes-like electric piano FM synthesis model
#include "Wurley.h"// Wurlitzer-like electric piano FM synthesis model
#include "TubeBell.h"// FM synthesis model
#include "HevyMetl.h"// Distorted synthesizer FM synthesis model
#include "PercFlut.h"// Percussive flute-like FM synthesis model
#include "BeeThree.h"// Cheezy organ FM synthesis model
#include "Moog.h"// Swept filter sampler
#include "FMVoices.h"// Three-formant FM voice synthesis
#include "VoicForm.h"// Four-formant resonance filter voice synthesis
#include "Resonate.h"// Noise through a BiQuad filter
#include "Drummer.h"// Sampling synthesis
#include "BandedWG.h"// Banded waveguide meta-object for bowed bars, tibetan bowls, etc.
#include "Shakers.h"// Various stochastic event models of shaker instruments
#include "ModalBar.h"// Various four-resonance presets (marimba, vibraphone, etc...)
#include "Mesh2D.h"// Two-dimensional, rectilinear digital waveguide mesh
#include "Whistle.h"// Hybrid physical/spectral model of a police whistle

enum STKInstrumentType{
    INSTRUMENT_CLARINET,
    INSTRUMENT_BLOWHOLE,
    INSTRUMENT_SAXOFONY,
    INSTRUMENT_FLUTE,
    INSTRUMENT_BRASS,
    INSTRUMENT_BLOWBOTL,
    INSTRUMENT_BOWED,
    INSTRUMENT_PLUCKED,
    INSTRUMENT_STIFKARP,
    INSTRUMENT_SITAR,
    INSTRUMENT_MANDOLIN,
    INSTRUMENT_RHODEY,
    INSTRUMENT_WURLEY,
    INSTRUMENT_TUBEBELL,
    INSTRUMENT_HEAVYMETL,
    INSTRUMENT_PERCFLUT,
    INSTRUMENT_BEETHREE,
    INSTRUMENT_MOOG,
    INSTRUMENT_FMVOICES,
    INSTRUMENT_VOICFORM,
    INSTRUMENT_RESONATE,
    INSTRUMENT_DRUMMER,
    INSTRUMENT_BANDEDWG,
    INSTRUMENT_SHAKERS,
    INSTRUMENT_MODALBAR,
    INSTRUMENT_MESH2D,
    INSTRUMENT_WHISTLE
};

enum STKInstrumentParam{
    
};

union STKInstrumentINSTR
{
    Clarinet* m_onezero;
    OnePole* m_onepole;
    PoleZero* m_polezero;
    TwoZero* m_twozero;
    TwoPole* m_twopole;
    BiQuad* m_biquad;
    FormSwep* m_formswep;
    Delay* m_delay;
    DelayL* m_delayl;
    DelayA* m_delaya;
    LentPitShift* m_lentpitshift;
    Chorus* m_chorus;
};


struct Instruments : public STKNode{
    STKInstrumentINSTR m_i;
	STKNode* m_source;
	Type m_type;
    std::function<StkFloat()> m_tickCallback;
};



// constructor
STKInstrument* STKInstrumentCreate(STKNode* source, Type type);

// destructor
STKInstrumentDelete(STKInstrument* i);

// functions

void STKInstrumentReset(STKInstrument* i){ i->m_outidx = 0; };
void STKInstrumentInit(STKInstrument* i);
void STKInstrumentTerm(STKInstrument* i);


// tick functions
inline StkFloat STKInstrumentTickClarinet(STKInstrument* i);
inline StkFloat STKInstrumentTickBlowHole(STKInstrument* i);
inline StkFloat STKInstrumentTickSaxophony(STKInstrument* i);
inline StkFloat STKInstrumentTickFlute(STKInstrument* i);
inline StkFloat STKInstrumentTickBrass()STKInstrument* i;
inline StkFloat STKInstrumentTickBlowBotl(STKInstrument* i);
inline StkFloat STKInstrumentTickBowed(STKInstrument* i);
inline StkFloat STKInstrumentTickPlucked(STKInstrument* i);
inline StkFloat STKInstrumentTickStiffKarp(STKInstrument* i);
inline StkFloat STKInstrumentTickSitar(STKInstrument* i);
inline StkFloat STKInstrumentTickMandolin(STKInstrument* i);
inline StkFloat STKInstrumentTickRhodey(STKInstrument* i);
inline StkFloat STKInstrumentTickWurley(STKInstrument* i);
inline StkFloat STKInstrumentTickTubeBell(STKInstrument* i);
inline StkFloat STKInstrumentTickHeavyMetl(STKInstrument* i);
inline StkFloat STKInstrumentTickPercFlut(STKInstrument* i);
inline StkFloat STKInstrumentTickBeeThree(STKInstrument* i);
inline StkFloat STKInstrumentTickMoog(STKInstrument* i);
inline StkFloat STKInstrumentTickFMVoices(STKInstrument* i);
inline StkFloat STKInstrumentTickVoicForm(STKInstrument* i);
inline StkFloat STKInstrumentTickResonate(STKInstrument* i);
inline StkFloat STKInstrumentTickDrummer(STKInstrument* i);
inline StkFloat STKInstrumentTickBandedWG(STKInstrument* i);
inline StkFloat STKInstrumentTickMoog(STKInstrument* i);
inline StkFloat STKInstrumentTickShakers(STKInstrument* i);
inline StkFloat STKInstrumentTickModalBar(STKInstrument* i);
inline StkFloat STKInstrumentTickMesh2D(STKInstrument* i);
inline StkFloat STKInstrumentTickWhistle(STKInstrument* i);
inline StkFloat STKInstrumentTickHasNoEffect(STKInstrument* i);
StkFloat STKInstrumentTick(STKInstrument* i, unsigned int channel = 0);

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT STKInstrumentType STKInstrumentGetType(){ return m_type; };
EXPORT void STKInstrumentSetType(STKInstrumentType type);
EXPORT void STKInstrumentSetScalar(StkFloat scalar, STKInstrumentParam param);
EXPORT void STKInstrumentSetHasNoEffect(bool hasnoeffect);

