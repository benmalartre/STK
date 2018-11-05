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

// ----------------------------------------------------------------------
//	STK GENERATOR STREAM
// ----------------------------------------------------------------------
int STKGeneratorStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{	
	STKGeneratorStream* generator = (STKGeneratorStream *)dataPointer;
	register StkFloat *samples = (StkFloat *)outputBuffer;

	switch (generator->m_type)
	{
	case STKGeneratorType::ADSR_GENERATOR:
	{
		ADSR * adsr = (ADSR*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = adsr->tick();
	}
	break;

	case STKGeneratorType::ASYMP_GENERATOR:
	{
		Asymp * asymp = (Asymp*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = asymp->tick();
	}
	break;

	case STKGeneratorType::BLITSAW_GENERATOR:
	{
		BlitSaw * blitsaw = (BlitSaw*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = blitsaw->tick();
	}
	break;

	case STKGeneratorType::BLITSQUARE_GENERATOR:
	{
		BlitSquare * blitsquare = (BlitSquare*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = blitsquare->tick();
	}
	break;

	case STKGeneratorType::BLIT_GENERATOR:
	{
		Blit * blit = (Blit*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = blit->tick();
	}
	break;

	case STKGeneratorType::ENVELOPE_GENERATOR:
	{
		Envelope * envelope = (Envelope*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = envelope->tick();
	}
	break;

	case STKGeneratorType::GRANULATE_GENERATOR:
	{
		Granulate * granulate = (Granulate*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = granulate->tick();
	}
	break;

	case STKGeneratorType::MODULATE_GENERATOR:
	{
		Modulate * modulate = (Modulate*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = modulate->tick();
	}
	break;

	case STKGeneratorType::NOISE_GENERATOR:
	{
		Noise * noise = (Noise*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = noise->tick();
	}
	break;

	case STKGeneratorType::SINEWAVE_GENERATOR:
	{
		SineWave * sinewave = (SineWave*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = sinewave->tick();
	}
	break;

	case STKGeneratorType::SINGWAVE_GENERATOR:
	{
		SingWave * singwave = (SingWave*)generator->m_generator;
		for (unsigned int i = 0; i < nBufferFrames; i++)
			*samples++ = singwave->tick();
	}
	break;

	//case STKGeneratorType::SUBNOISE_GENERATOR:
	//	SineWave * sinewave = (SineWave*)generator->m_generator;
	//	for (unsigned int i = 0; i < nBufferFrames; i++)
	//		sinewave->tick();
	//	break;
	}

	return 0;
}

STKGeneratorStream* STKGeneratorStreamSetup(RtAudio* DAC, STKGeneratorType _type, float _frequency)
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate(44100.0);

	STKGeneratorStream* generator = new STKGeneratorStream();
	generator->m_dac = DAC;
	switch (_type)
	{
	case STKGeneratorType::ADSR_GENERATOR:
	{
		ADSR* adsr = new ADSR();
		adsr->setValue(_frequency);
		generator->m_generator = (Generator*)adsr;
		generator->m_type = STKGeneratorType::ADSR_GENERATOR;
		break;
	}

	case STKGeneratorType::ASYMP_GENERATOR:
	{
		Asymp * asymp = new Asymp();
		asymp->setValue(_frequency);
		generator->m_generator = (Generator*)asymp;
		generator->m_type = STKGeneratorType::ASYMP_GENERATOR;
		break;
	}

	case STKGeneratorType::BLITSAW_GENERATOR:
	{
		BlitSaw * blitsaw = new BlitSaw(_frequency);
		generator->m_generator = (Generator*)blitsaw;
		generator->m_type = STKGeneratorType::BLITSAW_GENERATOR;
		break;
	}

	case STKGeneratorType::BLITSQUARE_GENERATOR:
	{
		BlitSquare * blitsquare = new BlitSquare(_frequency);
		generator->m_generator = (Generator*)blitsquare;
		generator->m_type = STKGeneratorType::BLITSQUARE_GENERATOR;
		break;
	}

	case STKGeneratorType::BLIT_GENERATOR:
	{
		Blit * blit = new Blit(_frequency);
		generator->m_generator = (Generator*)blit;
		generator->m_type = STKGeneratorType::BLIT_GENERATOR;
		break;
	}

	case STKGeneratorType::ENVELOPE_GENERATOR:
	{
		Envelope* envelope = new Envelope();
		generator->m_generator = (Generator*)envelope;
		generator->m_type = STKGeneratorType::ENVELOPE_GENERATOR;
		break;
	}

	case STKGeneratorType::GRANULATE_GENERATOR:
	{
		Granulate * granulate = new Granulate();
		generator->m_generator = (Generator*)granulate;
		generator->m_type = STKGeneratorType::GRANULATE_GENERATOR;
		break;
	}

	case STKGeneratorType::MODULATE_GENERATOR:
	{
		Modulate * modulate = new Modulate();
		generator->m_generator = (Generator*)modulate;
		generator->m_type = STKGeneratorType::MODULATE_GENERATOR;
		break;
	}

	case STKGeneratorType::NOISE_GENERATOR:
	{
		Noise * noise = new Noise(_frequency);
		generator->m_generator = (Generator*)noise;
		generator->m_type = STKGeneratorType::NOISE_GENERATOR;
		break;
	}

	case STKGeneratorType::SINEWAVE_GENERATOR:
	{
		SineWave * sinewave = new SineWave();
		sinewave->setFrequency(_frequency);
		generator->m_generator = (Generator*)sinewave;
		generator->m_type = STKGeneratorType::SINEWAVE_GENERATOR;
		break;
	}

	case STKGeneratorType::SINGWAVE_GENERATOR:
	{
		//SingWave * singwave = new SingWave(m_filename);
		//generator->m_generator = (Generator*)singwave;
		break;
	}
	}

	if (generator->m_generator)
	{
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
			delete generator->m_generator;
			delete generator;
			return NULL;
		}

		//generator->m_generator.setFrequency(_frequency);
		return generator;
	}

	delete generator;
	return NULL;
}

bool STKGeneratorStreamStart(STKGeneratorStream* _generator)
{
	try {
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

bool STKVoicerStreamStart(STKVoicerStream* _voicer)
{
	try {
		_voicer->m_dac->startStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKVoicerStreamStop(STKVoicerStream* _voicer)
{
	try {
		_voicer->m_dac->stopStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKVoicerStreamClean(STKVoicerStream* _voicer)
{
	// Shut down the output stream.
	try {
		_voicer->m_dac->closeStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

