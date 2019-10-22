#include "streams.h"

// ----------------------------------------------------------------------
//	STK GET NODE STREAM
// ----------------------------------------------------------------------
STKStream* STKGetStream(STKNode* node){
	return node->getStream();
}

// ----------------------------------------------------------------------
//	STK NODE IS ROOT
// ----------------------------------------------------------------------
bool STKIsRoot(STKNode* node){
	return node->isRoot();
}

// ----------------------------------------------------------------------
//	STK SET NODE AS ROOT (or NOT)
// ----------------------------------------------------------------------
void STKSetAsRoot(STKNode* node, bool isRoot)
{
	if (isRoot){
		node->setIsRoot(true);
		node->incrementNumOutput();
	}
	else{
		node->setIsRoot(false);
		node->decrementNumOutput();
	}
}

// ----------------------------------------------------------------------
//	STK ADD GENERATOR NODE
// ----------------------------------------------------------------------
STKNode* STKAddGenerator(STKStream* stream, STKGenerator::Type type, StkFloat frequency, bool isRoot)
{
	STKNode* node = (STKNode*)new STKGenerator(type, frequency);
	node->setStream(stream);
	node->incrementNumOutput();
    stream->m_nodes.push_back(node);
	if (isRoot)
	{
		node->setIsRoot(true);
		stream->m_roots.push_back(node);
	}

	else node->setIsRoot(false);
		
	return node;
}

// ----------------------------------------------------------------------
//	STK ADD ENVELOPE NODE
// ----------------------------------------------------------------------
STKNode* STKAddEnvelope(STKStream* stream, STKEnvelope::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKEnvelope(type, source);
	source->incrementNumOutput();
	if (isRoot)
	{
		node->setIsRoot(true);
		stream->m_roots.push_back(node);
	}
	else node->setIsRoot(false);

	return node;
}

// ----------------------------------------------------------------------
//	STK ADD ARYTHMETIC NODE
// ----------------------------------------------------------------------
STKNode* STKAddArythmetic(STKStream* stream, STKArythmetic::Mode mode, STKNode* lhs, STKNode* rhs, bool isRoot)
{
	STKNode* node = (STKNode*)new STKArythmetic(lhs, rhs, mode);
	if(lhs)lhs->incrementNumOutput();
	if(rhs)rhs->incrementNumOutput();

	node->incrementNumOutput();
	if (isRoot)
	{
	    node->setIsRoot(true);
	    stream->m_roots.push_back(node);
	}
	else node->setIsRoot(false);

	return node;
}

// ----------------------------------------------------------------------
//	STK ADD EFFECT NODE
// ----------------------------------------------------------------------
STKNode* STKAddEffect(STKStream* stream, STKEffect::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKEffect(source, type);
	
	source->incrementNumOutput();
	node->incrementNumOutput();
	if (isRoot)
	{
		node->setIsRoot(true);
		stream->m_roots.push_back(node);
	}
	else node->setIsRoot(false);
		
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD FILTER NODE
// ----------------------------------------------------------------------
STKNode* STKAddFilter(STKStream* stream, STKFilter::Type type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)new STKFilter(source, type);

	source->incrementNumOutput();
	node->incrementNumOutput();
	if (isRoot)
	{
		node->setIsRoot(true);
		stream->m_roots.push_back(node);
	}
	else node->setIsRoot(false);

	return node;
}

// ----------------------------------------------------------------------
//	STK ADD BUFFER NODE
// ----------------------------------------------------------------------
STKNode* STKAddBuffer(STKStream* stream, STKNode* previous, bool isRoot)
{
	STKNode* node = (STKNode*)new STKBuffer(previous);
	node->setStream(stream);
	node->incrementNumOutput();
	if (isRoot)
	{
		node->setIsRoot(true);
		stream->m_roots.push_back(node);
	}

	else node->setIsRoot(false);

	return node;
}

// ----------------------------------------------------------------------
//    STK ADD READER NODE
// ----------------------------------------------------------------------
STKNode* STKAddReader(STKStream* stream, const char* filename, bool isRoot)
{
    STKReader* reader = new STKReader();
    STKNode* node = static_cast<STKNode*>(reader);
    node->setStream(stream);
    node->incrementNumOutput();
    if (isRoot)
    {
        node->setIsRoot(true);
        stream->m_roots.push_back(node);
    }
    
    else node->setIsRoot(false);
    reader->setFile(filename);
    return node;
    
}

// ----------------------------------------------------------------------
//    STK REMOVE NODE
// ----------------------------------------------------------------------
void STKRemoveNode(STKStream* stream, STKNode* node)
{
    
    for (int n=0;n<stream->m_roots.size();++n)
    {
        if(stream->m_roots[n] == node)stream->m_roots.erase(stream->m_roots.begin()+n);
    }
    
    for (int n=0;n<stream->m_nodes.size();++n)
    {
        if(stream->m_nodes[n] == node)stream->m_nodes.erase(stream->m_nodes.begin()+n);
    }
    
    delete node;
}

void STKRemoveAllNode(STKStream* stream)
{
    
    for (int n=0;n<stream->m_roots.size();++n)
    {
        stream->m_roots.erase(stream->m_roots.begin()+n);
    }
    
    for (int n=0;n<stream->m_nodes.size();++n)
    {
        STKNode* node = stream->m_nodes[n];
        stream->m_nodes.erase(stream->m_nodes.begin()+n);
        delete node;
    }
}

// ----------------------------------------------------------------------
//	STK GENERATOR STREAM
// ----------------------------------------------------------------------
int STKStreamTick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *data)
{	
	STKStream* stream = (STKStream *)data;
	StkFloat *samples = (StkFloat *)outputBuffer;
	size_t numRoots = stream->m_roots.size();
	if (!numRoots)return 0;
	float weight = 1.0 / numRoots;
	for (unsigned int i = 0; i < nBufferFrames; i++)
	{
		*samples = 0;
		for (unsigned int j = 0; j < numRoots; ++j)
			*samples += stream->m_roots[j]->tick() * weight;
		samples++;

	}

	return 0;
}

STKStream* STKStreamSetup(RtAudio* DAC, int numChannels)
{
	STKStream* stream = new STKStream();
	stream->m_dac = DAC;

	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = DAC->getDefaultOutputDevice();
	parameters.nChannels = numChannels;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		DAC->openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &STKStreamTick, (void *)stream);
	}
	catch (RtAudioError &error) {
		error.printMessage();
		delete stream;
		return NULL;
	}

	return stream;
}

bool STKStreamStart(STKStream* stream)
{
	try {
		
		//for (std::vector<STKNode*>::iterator it = _generator->m_roots.begin(); it < _generator->m_roots.end(); it++)
		//	(*it)->reset();
		
		stream->m_dac->startStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

bool STKStreamStop(STKStream* stream)
{
    if(stream->m_dac){
        try {
            stream->m_dac->stopStream();
        }
        catch (RtAudioError &error) {
            error.printMessage();
            return false;
        }
	    return true;
    }
    else return false;
}

bool STKStreamClean(STKStream* stream)
{
	// Shut down the output stream.
	try {
		stream->m_dac->closeStream();
	}
	catch (RtAudioError &error) {
		error.printMessage();
		return false;
	}
	return true;
}

int STKStreamNumRoots(STKStream* stream)
{
    return (int)stream->m_roots.size();
}

/*
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
*/
