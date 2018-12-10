#include "CAPI.h"

#include "ADSR.h"
#include "Asymp.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Blit.h"
#include "Envelope.h"
#include "Granulate.h"
#include "Modulate.h"
#include "Noise.h"
#include "SineWave.h"
#include "SingWave.h"


// ----------------------------------------------------------------------
//	STK GENERAL FUNCTIONS
// ----------------------------------------------------------------------
RtAudio* STKInit()
{
	try {
		RtAudio* DAC = new RtAudio();
		return DAC;
	}
	catch (RtAudioError &error) {
		return NULL;
	}
	
}

void STKTerm(RtAudio* DAC)
{
	DAC->closeStream();
	delete DAC;
}

void STKSetSampleRate(float _rate)
{
	Stk::setSampleRate(_rate);
}

int STKGetDevices(RtAudio* DAC)
{
	if (DAC != NULL) {
		int nb = DAC->getDeviceCount();
		for (int i = 0; i<nb; i++) {
			RtAudio::DeviceInfo info;
			info = DAC->getDeviceInfo(i);
		}
		return nb;
	}
	else
		return 0;
}

//--------------------------------------------------------------------
// STKGenerator Node Constructor
//--------------------------------------------------------------------
STKGenerator::STKGenerator(Type type, float frequency)
{
	m_type = type;
	m_frequency = frequency;
	init();
}

//--------------------------------------------------------------------
// STKGenerator Node Terminate
//--------------------------------------------------------------------
void STKGenerator::term()
{
	if (m_adsr)delete m_adsr;
	if (m_asymp)delete m_asymp;
	if (m_blit)delete m_blit;
	if (m_blitsaw)delete m_blitsaw;
	if (m_blitsquare)delete m_blitsquare;
	if (m_envelope)delete m_envelope;
	if (m_granulate)delete m_granulate;
	if (m_modulate)delete m_modulate;
	if (m_noise)delete m_noise;
	if (m_sinewave)delete m_sinewave;
	if (m_singwave)delete m_singwave;
}

//--------------------------------------------------------------------
// STKGenerator Node Init
//--------------------------------------------------------------------
void STKGenerator::init()
{
	switch (m_type)
	{
	case ADSR_GENERATOR:
	{
		m_adsr = new ADSR();
		m_adsr->setValue(m_frequency);
		m_type = ADSR_GENERATOR;
		m_tickCallback = [this](){ return this->ADSRTickCallback(); };
		break;
	}

	case ASYMP_GENERATOR:
	{
		m_asymp = new Asymp();
		m_asymp->setValue(m_frequency);
		m_type = ASYMP_GENERATOR;
		m_tickCallback = [this](){ return this->AsympTickCallback(); };
		break;
	}

	case BLITSAW_GENERATOR:
	{
		m_blitsaw = new BlitSaw(m_frequency);
		m_type = BLITSAW_GENERATOR;
		m_tickCallback = [this](){ return this->BlitSawTickCallback(); };
		break;
	}

	case BLITSQUARE_GENERATOR:
	{
		m_blitsquare = new BlitSquare(m_frequency);
		m_type = BLITSQUARE_GENERATOR;
		m_tickCallback = [this](){ return this->BlitSquareTickCallback(); };
		break;
	}

	case BLIT_GENERATOR:
	{
		m_blit = new Blit(m_frequency);
		m_type = BLIT_GENERATOR;
		m_tickCallback = [this](){ return this->BlitTickCallback(); };
		break;
	}

	case ENVELOPE_GENERATOR:
	{
		m_envelope = new Envelope();
		m_type = ENVELOPE_GENERATOR;
		m_tickCallback = [this](){ return this->EnvelopeTickCallback(); };
		break;
	}

	case GRANULATE_GENERATOR:
	{
		m_granulate = new Granulate();
		m_type = GRANULATE_GENERATOR;
		m_tickCallback = [this](){ return this->GranulateTickCallback(); };
		break;
	}

	case MODULATE_GENERATOR:
	{
		m_modulate = new Modulate();
		m_type = MODULATE_GENERATOR;
		m_tickCallback = [this](){ return this->ModulateTickCallback(); };
		break;
	}

	case NOISE_GENERATOR:
	{
		m_noise = new Noise(m_frequency);
		m_type = NOISE_GENERATOR;
		m_tickCallback = [this](){ return this->NoiseTickCallback(); };
		break;
	}

	case SINEWAVE_GENERATOR:
	{
		m_sinewave = new SineWave();
		m_sinewave->setFrequency(m_frequency);
		m_type = SINEWAVE_GENERATOR;
		m_tickCallback = [this](){ return this->SineWaveTickCallback(); };
		break;
	}

	case SINGWAVE_GENERATOR:
	{
		/*
		m_singwave = new SingWave();
		m_singwave->setFrequency(frequency);
		m_type = STKGeneratorType::SINGWAVE_GENERATOR;
		m_tickCallback = [this](){ return this->SingWaveTickCallback(); };
		break;
		*/
	}
	}
}


//--------------------------------------------------------------------
// STKGenerator Node Destructor
//--------------------------------------------------------------------
STKGenerator::~STKGenerator()
{
	term();
}

//--------------------------------------------------------------------
// STKGenerator Node Set Frequency
//--------------------------------------------------------------------
void STKGenerator::setFrequency(float frequency)
{
	if (m_type == SINEWAVE_GENERATOR)
	{
		m_sinewave->setFrequency(frequency);
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Reset
//--------------------------------------------------------------------
void STKGenerator::reset()
{
	switch (m_type)
	{
		case SINEWAVE_GENERATOR:
		{
			m_sinewave->reset();
			break;
		}

		case BLIT_GENERATOR:
		{
			m_blit->reset();
			break;
		}

		case BLITSAW_GENERATOR:
		{
			m_blitsaw->reset();
			break;
		}

		case BLITSQUARE_GENERATOR:
		{
			m_blitsquare->reset();
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Set Type
//--------------------------------------------------------------------
void STKGenerator::setType(Type type)
{
	if (type != m_type)
	{
		term();
		m_type = type;
		init();
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Tick()
//--------------------------------------------------------------------
StkFloat STKGenerator::EnvelopeTickCallback()
{
	return m_envelope->tick();
}
StkFloat STKGenerator::ADSRTickCallback()
{
	return m_adsr->tick();
}
StkFloat STKGenerator::AsympTickCallback()
{
	return m_asymp->tick();
}
StkFloat STKGenerator::NoiseTickCallback()
{
	return m_noise->tick();
}

StkFloat STKGenerator::ModulateTickCallback()
{
	return m_modulate->tick();
}

StkFloat STKGenerator::SingWaveTickCallback()
{
	return m_singwave->tick();
}

StkFloat STKGenerator::SineWaveTickCallback()
{
	return m_sinewave->tick();
}

StkFloat STKGenerator::BlitTickCallback()
{
	return m_blit->tick();
}

StkFloat STKGenerator::BlitSawTickCallback()
{
	return m_blitsaw->tick();
}

StkFloat STKGenerator::BlitSquareTickCallback()
{
	return m_blitsquare->tick();
}

StkFloat STKGenerator::GranulateTickCallback()
{
	return m_granulate->tick();
}

StkFloat STKGenerator::tick(unsigned int channel)
{
	return m_tickCallback();
}

//--------------------------------------------------------------------
// STKArythmetic Node Constructor
//--------------------------------------------------------------------
STKArythmetic::STKArythmetic(STKNode* a, STKNode* b, Mode mode)
{
	m_lhs = a;
	m_rhs = b;
	m_mode = mode;
	init();
}

//--------------------------------------------------------------------
// STKArythmetic Set Mode
//--------------------------------------------------------------------
void STKArythmetic::setMode(Mode mode)
{
	if (mode != m_mode)
	{
		term();
		m_mode = mode;
		init();
	}
}

//--------------------------------------------------------------------
// STKArythmetic Node Init
//--------------------------------------------------------------------
void STKArythmetic::init()
{
	switch (m_mode)
	{
		case Mode::ADD:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickAdd(); };
			break;
		}
		case Mode::SUB:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickSub(); };
			break;
		}
		case Mode::MULTIPLY:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickMultiply(); };
			break;
		}
		case Mode::SCALE:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScale(); };
			break;
		}
		case Mode::SCALEADD:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScaleAdd(); };
			break;
		}
		case Mode::SCALESUB:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickScaleSub(); };
			break;
		}
		case Mode::MIX:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickMix(); };
			break;
		}
		case Mode::BLEND:
		{
			m_tickCallback = [this](){ return this->ArythmeticTickBlend(); };
			break;
		}
	}
}

void STKArythmetic::term()
{
}


//--------------------------------------------------------------------
// STKArythmetic Node Tick()
//--------------------------------------------------------------------
StkFloat STKArythmetic::ArythmeticTickAdd()
{
	return m_lhs->tick() + m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickSub()
{
	return m_lhs->tick() - m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickMultiply()
{
	return m_lhs->tick() * m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickScale()
{
	return m_lhs->tick() * m_scalar;
}

StkFloat STKArythmetic::ArythmeticTickScaleAdd()
{
	return m_lhs->tick() + m_scalar * m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickScaleSub()
{
	return m_lhs->tick() - m_scalar * m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickMix()
{
	return (1.0-m_scalar) * m_lhs->tick() + m_scalar * m_rhs->tick();
}

StkFloat STKArythmetic::ArythmeticTickBlend()
{
	return (m_lhs->tick() + m_rhs->tick()) * 0.5f;
}

StkFloat STKArythmetic::tick(unsigned int channel)
{
	return m_tickCallback();
}

//--------------------------------------------------------------------
// STKEffect Node Tick()
//--------------------------------------------------------------------
StkFloat STKEffect::tick(unsigned int channel)
{
	switch (m_type)
	{
		case ECHO:
		{
			Echo* echo = (Echo*)m_effect;
			return echo->tick(m_source->tick());
			break;
		}
		case PITSHIFT:
		{
			PitShift* pitshift = (PitShift*)m_effect;
			return pitshift->tick(m_source->tick());
			break;
		}
		case LENTPITSHIFT:
		{
			LentPitShift* lentpitshift = (LentPitShift*)m_effect;
			return lentpitshift->tick(m_source->tick());
			break;
		}
		case CHORUS:
		{
			Chorus* chorus = (Chorus*)m_effect;
			return chorus->tick(m_source->tick());
			break;
		}
		default:
		{
			return 0.0;
		}
	}
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD GENERATOR NODE
// ----------------------------------------------------------------------
STKGenerator* STKAddGenerator(STKGeneratorStream* generator, STKGenerator::Type type, StkFloat frequency, bool isRoot)
{
	STKGenerator* node = new STKGenerator(type, frequency);

	generator->m_nodes.push_back(node);
	if (isRoot)
		generator->m_roots.push_back(node);
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD ARYTHMETIC NODE
// ----------------------------------------------------------------------
STKArythmetic* STKAddArythmetic(STKGeneratorStream* generator, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot)
{
	STKArythmetic* node = new STKArythmetic(lhs, rhs, mode);
	generator->m_nodes.push_back(node);
	if (isRoot)
		generator->m_roots.push_back(node);
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD EFFECT NODE
// ----------------------------------------------------------------------
STKEffect* STKAddEffect(STKGeneratorStream* generator, STKEffect::Type type, STKNode* source, bool isRoot)
{
	STKEffect* node = new STKEffect(source, type);
	generator->m_nodes.push_back(node);
	if (isRoot)
		generator->m_roots.push_back(node);
	return node;
}

// ----------------------------------------------------------------------
//	SETTERS
// ----------------------------------------------------------------------
void STKSetGeneratorType(STKGenerator* generator, STKGenerator::Type type)
{
	generator->setType(type);
}

void STKSetArythmeticMode(STKArythmetic* arythmetic, STKArythmetic::Mode mode)
{
	arythmetic->setMode(mode);
}

void STKSetArythmeticScalar(STKArythmetic* generator, StkFloat scalar)
{
	generator->setScalar(scalar);
}

void STKSetEffectType(STKEffect* effect, STKEffect::Type type)
{

}

// ----------------------------------------------------------------------
//	STK GENERATOR STREAM
// ----------------------------------------------------------------------
int STKGeneratorStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{	
	STKGeneratorStream* generator = (STKGeneratorStream *)dataPointer;
	register StkFloat *samples = (StkFloat *)outputBuffer;
	unsigned int numRoots = generator->m_roots.size();
	if (!numRoots)return 0;
	float weight = 1.0 / numRoots;
	for (unsigned int i = 0; i < nBufferFrames; i++)
	{
		*samples = 0;
		for (unsigned int j = 0; j < numRoots; ++j)
			*samples += generator->m_roots[j]->tick();
		*samples *= weight;
		samples++;

	}

	return 0;
}

STKGeneratorStream* STKGeneratorStreamSetup(RtAudio* DAC, STKGenerator::Type _type, float _frequency)
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate(44100.0);

	STKGeneratorStream* generator = new STKGeneratorStream();
	generator->m_dac = DAC;
	
	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = DAC->getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		DAC->openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &STKGeneratorStreamTick, (void *)generator);
	}
	catch (RtAudioError &error) {
		error.printMessage();
		//delete generator->m_generator;
		delete generator;
		return NULL;
	}

	return generator;
}

bool STKGeneratorStreamStart(STKGeneratorStream* _generator)
{
	try {
		
		//for (std::vector<STKNode*>::iterator it = _generator->m_roots.begin(); it < _generator->m_roots.end(); it++)
		//	(*it)->reset();
		
		_generator->m_dac->startStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKGeneratorStreamStop(STKGeneratorStream* _generator)
{
	try {
		_generator->m_dac->stopStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKGeneratorStreamClean(STKGeneratorStream* _generator)
{
	// Shut down the output stream.
	try {
		_generator->m_dac->closeStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------
//	STK INPUT STREAM
// ----------------------------------------------------------------------
int STKInputStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *data)
{
	//if ( status ) std::cout << "Stream over/underflow detected." << std::endl;

	STKInputStream* buffer = (STKInputStream*)data;
	StkFloat* output = (StkFloat*)outputBuffer;

	StkFloat* visu = output;
	unsigned int i, j;
	float inv_freq;
	float *lastValues;

	if (buffer->m_initialized) {
		unsigned int count = fread(outputBuffer, buffer->m_nbChannels * sizeof(StkFloat), buffer->m_nbFrames, buffer->m_file);
		if (count < buffer->m_nbFrames) {
			unsigned int bytes = (buffer->m_nbFrames - count) * buffer->m_nbChannels * sizeof(StkFloat);
			unsigned int startByte = count * buffer->m_nbChannels * sizeof(StkFloat);
			memset((char *)(outputBuffer)+startByte, 0, bytes);
			return 1;
		}
	}

	// Vizualisation 
	memcpy(buffer->m_buffer, visu, buffer->m_nbFrames * buffer->m_nbChannels * sizeof(StkFloat));

	return 0;
}

void STKInputStreamSetFile(STKInputStream* buffer, const char* file_name) 
{
	if (buffer->m_file)fclose(buffer->m_file);
	buffer->m_file = fopen(file_name, "rb");
	if (file_name != NULL) {
		if (!buffer->m_file) {
			buffer->m_initialized = false;
			std::cout << "Unable to find or open file!\n";
		}
		else buffer->m_initialized = true;

	}
	else {
		std::cout << "No file name specified!\n";
	}
}

void STKInputStreamSetFrequency(STKInputStream* buffer, float frequency) 
{
	buffer->m_frequency = frequency;
}

STKInputStream* STKInputStreamInit(RtAudio* DAC) 
{

	if (DAC == NULL)return NULL;


	STKInputStream* buffer = new STKInputStream();
	buffer->m_nbFrames = 512;
	buffer->m_nbChannels = 2;
	buffer->m_sampleRate = 44100;
	buffer->m_iParams.deviceId = DAC->getDefaultInputDevice(); // first available device
	buffer->m_iParams.nChannels = buffer->m_nbChannels;
	buffer->m_oParams.deviceId = DAC->getDefaultOutputDevice(); // first available device
	buffer->m_oParams.nChannels = buffer->m_nbChannels;

	buffer->m_frequency = 440.0;

	//try{
	//	DAC->openStream( &buffer->oParams, NULL, RTAUDIO_FLOAT64,buffer->sampleRate, &buffer->nbFrames, &saw, (void *)buffer );
	//    	DAC->startStream();
	//}

	try {
		DAC->openStream(&buffer->m_oParams,
			&buffer->m_iParams,
			FORMAT,
			buffer->m_sampleRate,
			&buffer->m_nbFrames,
			&STKInputStreamTick,
			(void *)buffer);
		DAC->startStream();
	}

	catch (RtAudioError& e) {
		//e.printMessage();
		return NULL;
	}
	buffer->m_buffer = new StkFloat[buffer->m_nbFrames * buffer->m_nbChannels];
	memset(buffer->m_buffer, 0, buffer->m_nbFrames * buffer->m_nbChannels * sizeof(StkFloat));
	return buffer;
}

void STKInputStreamGet(STKInputStream* buffer, StkFloat* output) {
	memcpy(output, buffer->m_buffer, buffer->m_nbFrames*buffer->m_nbChannels * sizeof(StkFloat));
}

void STKInputStreamTerm(STKInputStream* buffer) {
	//DAC->closeStream();
	if (buffer->m_file != NULL)fclose(buffer->m_file);
	delete buffer;
}

// ----------------------------------------------------------------------
//	STK VOICER STREAM
// ----------------------------------------------------------------------
int STKVoicerStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
	STKVoicerStream* voicer = (STKVoicerStream *)dataPointer;
	if (voicer->m_settling == true)goto settle;

	register StkFloat  sample, *samples = (StkFloat *)outputBuffer;

	if (voicer->m_nbInstruments == 0)
	{
		return 1;
	}

	for (ULONG i = 0; i<voicer->m_nbInstruments; i++)
	{
		/*
		InstrumentData* currentInstru = data->instrumentsData[i];
		if (!currentInstru->enable)
		{
			data->instruments[i]->noteOff(64.0);
			//data->instruments[i]->s
			continue;
		}

		if (currentInstru->changeInstrument)
		{
			//Application().LogMessage(L"Change Instrument ID "+(CString)i +" Called...");
			data->voicer->removeInstrument(data->instruments[i]);
			delete data->instruments[i];

			Instrmnt* instrmnt = voiceByNumber3(currentInstru->instrumentCurrent);
			if (instrmnt == NULL)
				instrmnt = voiceByNumber3(7);
			data->voicer->addInstrument(instrmnt, i);
			data->instruments[i] = instrmnt;

		}

		//data->instruments[i]->setFrequency(currentInstru->frequency);
		if (currentInstru->noteOn)
		{
			currentInstru->noteTag = data->voicer->noteOn(currentInstru->note, currentInstru->volume, i);
			currentInstru->noteOn = false;
		}
		else
		{
		data->voicer->noteOff((LONG)currentInstru->noteTag,64.0);
		}
		*/
	}

	for (unsigned int i = 0; i<nBufferFrames; i++)
	{
		sample = voicer->m_volume * voicer->m_voicer->tick();
		*samples++ = sample;
	}

	voicer->m_counter++;

	if (voicer->m_counter>voicer->m_maxTick)
	{
		voicer->m_settling = true;
		goto settle;
	}

	return 1;

settle:
	//voicer->m_voicer->silence();
	voicer->m_dac->abortStream();

	return 0;

}


STKVoicerStream* STKVoicerStreamSetup(RtAudio* DAC, int _nbInstruments)
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate(44100.0);

	STKVoicerStream* voicer = new STKVoicerStream();
	voicer->m_dac = DAC;
	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = voicer->m_dac->getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		voicer->m_dac->openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &STKVoicerStreamTick, (void *)&voicer->m_instruments[0]);
	}
	catch (RtAudioError &error) {
		error.printMessage();
		delete voicer;
		return NULL;
	}

	return voicer;
}

bool STKVoicerStreamStart(STKVoicerStream* voicer)
{
	try {
		voicer->m_dac->startStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKVoicerStreamStop(STKVoicerStream* voicer)
{
	try {
		voicer->m_dac->stopStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKVoicerStreamClean(STKVoicerStream* voicer)
{
	// Shut down the output stream.
	try {
		voicer->m_dac->closeStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

