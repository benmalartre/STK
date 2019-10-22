
#ifndef STK_GENERATORS_H
#define STK_GENERATORS_H

#include "common.h"
#include "Envelope.h"
#include "ADSR.h"
#include "Asymp.h"
#include "Noise.h"
#include "SingWave.h"
#include "SineWave.h"
#include "Blit.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Modulate.h"
#include "Granulate.h"

/*
ENVELOPE_GENERATOR,
ADSR_GENERATOR,
MODULATE_GENERATOR,
GRANULATE_GENERATOR
*/
class STKGenerator : public STKNode{
public:
	enum Type {
		ASYMP_GENERATOR,
		NOISE_GENERATOR,
		BLIT_GENERATOR,
		BLITSAW_GENERATOR,
		BLITSQUARE_GENERATOR,
		SINEWAVE_GENERATOR
        /*,
        SINGWAVE_GENERATOR,
		MODULATE_GENERATOR,
		GRANULATE_GENERATOR
        */
	};

	enum Param {
		T60,					// asymp t60
		TARGET,					// asymp target
		TAU,					// asymp tau
		TIME,					// asymp time
		VALUE,					// asymp value
		FREQUENCY,				// waves frequency
		HARMONICS,				// waves harmonics
		PHASE,					// waves phase
		PHASEOFFSET,			// sine wave phase offset
		SEED,					// noise seed
	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKGenerator(Type type, StkFloat frequency=440.0f);
    STKGenerator(const STKGenerator& other);
	// destructor 
	~STKGenerator();
	// overrides
	void reset();
	void init();
	void term();
	// functions
	Type getType(){ return m_type; };
	void setType(Type type);
	void setScalar(Param param, StkFloat scalar);
	void setHasNoEffect(bool hasnoeffect);

private:
	union
	{
		Asymp* m_asymp;
		Noise* m_noise;
		Blit* m_blit;
		BlitSaw* m_blitsaw;
		BlitSquare* m_blitsquare;
		SineWave* m_sinewave;
		SingWave* m_singwave;
		Modulate* m_modulate;
		Granulate* m_granulate;
	};
	inline StkFloat GeneratorTickAsymp();
	inline StkFloat GeneratorTickNoise();
	inline StkFloat GeneratorTickBlit();
	inline StkFloat GeneratorTickBlitSaw();
	inline StkFloat GeneratorTickBlitSquare();
	inline StkFloat GeneratorTickSineWave();
	inline StkFloat SingWaveTickCallback();
	inline StkFloat EnvelopeTickModulate();
	inline StkFloat EnvelopeTickGranulate();
	inline StkFloat GeneratorTickHasNoEffect();
	
	
	
	std::function<StkFloat()> m_tickCallback;
	Type m_type;
	float m_frequency;
	
};

EXPORT void STKSetGeneratorType(STKGenerator* generator, STKGenerator::Type type);
EXPORT void STKSetGeneratorScalar(STKGenerator* generator, STKGenerator::Param param, StkFloat scalar);
EXPORT int STKGetGeneratorType(STKGenerator* generator);
#endif
