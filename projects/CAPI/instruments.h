#ifndef STK_CAPI_INSTRUMENT_H
#define STK_CAPI_INSTRUMENT_H

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

union STKInstrument
{
  Clarinet* m_clarinet;
  BlowHole* m_blowhole;
  Saxofony* m_saxofony;
  Flute* m_flute;
  Brass* m_brass;
  BlowBotl* m_blowbtl;
  Bowed* m_bowed;
  Plucked* m_plucked;
  StifKarp* m_stifkarp;
  Sitar* m_sitar;
  Mandolin* m_mandolin;
  Rhodey* m_rhodey;
  Wurley* m_wurley;
  TubeBell* m_tubebell;
  HevyMetl* m_hevymtl;
  PercFlut* m_percflut;
  BeeThree* m_beethree;
  Moog* m_moog;
  FMVoices* m_fmvoices;
  VoicForm* m_voicform;
  Resonate* m_resonate;
  Drummer* m_drummer;
  BandedWG* m_bandedwg;
  Shakers* m_shakers;
  ModalBar* m_modelbar;
  Mesh2D* m_mesh2D;
  Whistle* m_whistle;
};


struct STKInstruments : public STKNode{
  STKInstrument* m_i;
  STKNode* m_source;
  STKInstrumentType m_instrumentType;
  std::function<StkFloat()> m_tickCallback;
};



// constructor
STKInstruments* STKInstrumentCreate(STKNode* source, STKInstrumentType type);

// destructor
void STKInstrumentDelete(STKInstruments* i);

// functions

void STKInstrumentReset(STKInstruments* i);
void STKInstrumentInit(STKInstruments* i);
void STKInstrumentTerm(STKInstruments* i);

// tick functions
inline StkFloat STKInstrumentTickClarinet(STKInstruments* i);
inline StkFloat STKInstrumentTickBlowHole(STKInstruments* i);
inline StkFloat STKInstrumentTickSaxophony(STKInstruments* i);
inline StkFloat STKInstrumentTickFlute(STKInstruments* i);
inline StkFloat STKInstrumentTickBrass(STKInstruments* i);
inline StkFloat STKInstrumentTickBlowBotl(STKInstruments* i);
inline StkFloat STKInstrumentTickBowed(STKInstruments* i);
inline StkFloat STKInstrumentTickPlucked(STKInstruments* i);
inline StkFloat STKInstrumentTickStifKarp(STKInstruments* i);
inline StkFloat STKInstrumentTickSitar(STKInstruments* i);
inline StkFloat STKInstrumentTickMandolin(STKInstruments* i);
inline StkFloat STKInstrumentTickRhodey(STKInstruments* i);
inline StkFloat STKInstrumentTickWurley(STKInstruments* i);
inline StkFloat STKInstrumentTickTubeBell(STKInstruments* i);
inline StkFloat STKInstrumentTickHeavyMetl(STKInstruments* i);
inline StkFloat STKInstrumentTickPercFlut(STKInstruments* i);
inline StkFloat STKInstrumentTickBeeThree(STKInstruments* i);
inline StkFloat STKInstrumentTickMoog(STKInstruments* i);
inline StkFloat STKInstrumentTickFMVoices(STKInstruments* i);
inline StkFloat STKInstrumentTickVoicForm(STKInstruments* i);
inline StkFloat STKInstrumentTickResonate(STKInstruments* i);
inline StkFloat STKInstrumentTickDrummer(STKInstruments* i);
inline StkFloat STKInstrumentTickBandedWG(STKInstruments* i);
inline StkFloat STKInstrumentTickMoog(STKInstruments* i);
inline StkFloat STKInstrumentTickShakers(STKInstruments* i);
inline StkFloat STKInstrumentTickModalBar(STKInstruments* i);
inline StkFloat STKInstrumentTickMesh2D(STKInstruments* i);
inline StkFloat STKInstrumentTickWhistle(STKInstruments* i);
inline StkFloat STKInstrumentTickHasNoEffect(STKInstruments* i);
StkFloat STKInstrumentTick(STKInstruments* i, unsigned int channel = 0);

// ----------------------------------------------------------------------
//	STK FILTER NODE SETTER
// ----------------------------------------------------------------------
EXPORT STKInstrumentType STKInstrumentGetType(STKInstruments* i);
EXPORT void STKInstrumentSetType(STKInstrumentType type);
EXPORT void STKInstrumentSetScalar(float scalar, STKInstrumentParam param);
EXPORT void STKInstrumentSetHasNoEffect(bool hasnoeffect);

#endif // STK_CAPI_INSTRUMENT_H