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


class Instruments : public STKNode{
public:
	enum Type{
		CLARINET,
		BLOWHOLE,
		SAXOFONY,
		FLUTE,
		BRASS,
		BLOWBOTL,
		BOWED,
		PLUCKED,
		STIFKARP,
		SITAR,
        MANDOLIN,
        RHODEY,
        WURLEY,
        TUBEBELL,
        HEAVYMETL,
        PERCFLUT,
        BEETHREE,
        MOOG,
        FMVOICES,
        VOICFORM,
        RESONATE,
        DRUMMER,
        BANDEDWG,
        SHAKERS,
        MODALBAR,
        MESH2D,
        WHISTLE
	};

	enum Param{
    
	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKInstrument(STKNode* source, Type type);
	// destructor 
	~SSTKInstrument();
	// overrides
	void reset(){ m_outidx = 0; };
	void init();
	void term();
	// functions
	Type getType(){ return m_type; };
	void setType(Type type);
	void setScalar(StkFloat scalar, Param param);
	void setHasNoEffect(bool hasnoeffect);
private:
	inline StkFloat InstrumentTickClarinet();
	inline StkFloat InstrumentTickBlowHole();
	inline StkFloat InstrumentTickSaxophony();
	inline StkFloat InstrumentTickFlute();
	inline StkFloat InstrumentTickBrass();
	inline StkFloat InstrumentTickBlowBotl();
	inline StkFloat InstrumentTickBowed();
	inline StkFloat InstrumentTickPlucked();
	inline StkFloat InstrumentTickStiffKarp();
	inline StkFloat InstrumentTickSitar();
    inline StkFloat InstrumentTickMandolin();
    inline StkFloat InstrumentTickRhodey();
    inline StkFloat InstrumentTickWurley();
    inline StkFloat InstrumentTickTubeBell();
    inline StkFloat InstrumentTickHeavyMetl();
    inline StkFloat InstrumentTickPercFlut();
    inline StkFloat InstrumentTickBeeThree();
    inline StkFloat InstrumentTickMoog();
    inline StkFloat InstrumentTickFMVoices();
    inline StkFloat InstrumentTickVoicForm();
    inline StkFloat InstrumentTickResonate();
    inline StkFloat InstrumentTickDrummer();
    inline StkFloat InstrumentTickBandedWG();
    inline StkFloat InstrumentTickMoog();
    inline StkFloat InstrumentTickShakers();
    inline StkFloat InstrumentTickModalBar();
    inline StkFloat InstrumentTickMesh2D();
    inline StkFloat InstrumentTickWhistle();
	inline StkFloat InstrumentTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	union
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
	STKNode* m_source;
	Type m_type;
};

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT void STKSetFilterType(STKFilter* filter, STKFilter::Type type);
EXPORT void STKSetFilterScalar(STKFilter* filter, STKFilter::Param param, StkFloat scalar);

