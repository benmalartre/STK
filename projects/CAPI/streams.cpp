#include "streams.h"

// ----------------------------------------------------------------------
//	STK GENERATOR ADD GENERATOR NODE
// ----------------------------------------------------------------------
STKNode* STKAddGenerator(STKGeneratorStream* generator, STKGenerator::Type type, StkFloat frequency, bool isRoot)
{
	STKNode* node = (STKNode*)new STKGenerator(type, frequency);
	generator->m_nodes.push_back(node);
	if (isRoot)
	{
		node->incrementNumOutput();
		generator->m_roots.push_back(node);
	}
		
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD ENVELOPE NODE
// ----------------------------------------------------------------------
STKNode* STKAddEnvelope(STKGeneratorStream* generator, STKEnvelope::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKEnvelope(type, source);
	source->incrementNumOutput();
	generator->m_nodes.push_back(node);
	if (isRoot)
	{
		node->incrementNumOutput();
		generator->m_roots.push_back(node);
	}

	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD ARYTHMETIC NODE
// ----------------------------------------------------------------------
STKNode* STKAddArythmetic(STKGeneratorStream* generator, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot)
{
	STKNode* node = (STKNode*)new STKArythmetic(lhs, rhs, mode);
	lhs->incrementNumOutput();
	rhs->incrementNumOutput();

	generator->m_nodes.push_back(node);
	if (isRoot)
	{
		node->incrementNumOutput();
		generator->m_roots.push_back(node);
	}
		
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD EFFECT NODE
// ----------------------------------------------------------------------
STKNode* STKAddEffect(STKGeneratorStream* generator, STKEffect::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKEffect(source, type);
	
	source->incrementNumOutput();
	generator->m_nodes.push_back(node);
	if (isRoot)
	{
		node->incrementNumOutput();
		generator->m_roots.push_back(node);
	}
		
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD FILTER NODE
// ----------------------------------------------------------------------
STKNode* STKAddFilter(STKGeneratorStream* generator, STKFilter::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKFilter(source, type);

	source->incrementNumOutput();
	generator->m_nodes.push_back(node);
	if (isRoot)
	{
		node->incrementNumOutput();
		generator->m_roots.push_back(node);
	}

	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR STREAM
// ----------------------------------------------------------------------
int STKGeneratorStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{	
	STKGeneratorStream* generator = (STKGeneratorStream *)dataPointer;
	StkFloat *samples = (StkFloat *)outputBuffer;
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

STKGeneratorStream* STKGeneratorStreamSetup(RtAudio* DAC)
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
//	STK VOICER STREAM
// ----------------------------------------------------------------------
int STKVoicerStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
	STKVoicerStream* voicer = (STKVoicerStream *)dataPointer;
	if (voicer->m_settling == true){
        voicer->m_dac->abortStream();
        return 0;   
    }

	StkFloat  sample, *samples = (StkFloat *)outputBuffer;

	if (voicer->m_nbInstruments == 0)
	{
		return 1;
	}

	for (int i = 0; i<voicer->m_nbInstruments; i++)
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
        voicer->m_dac->abortStream();
        return 0;
	}

	return 1;

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

