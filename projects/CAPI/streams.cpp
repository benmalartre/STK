#include "streams.h"
#include "nodes.h"
#include "readers.h"
#include "exports.h"

// ----------------------------------------------------------------------
//    ROOT NODES
// ----------------------------------------------------------------------
void STKStreamRemoveRootNode(STKStream* stream, STKNode* node){
    stream->m_roots.push_back(node);
}

void STKStreamAddRootNode(STKStream* stream, STKNode* node){
    for(int i=0;i<stream->m_roots.size();i++){
        if(stream->m_roots[i] == node){
            stream->m_roots.erase(stream->m_roots.begin()+i);
        }
    }
}

// ----------------------------------------------------------------------
//	STK ADD GENERATOR NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddGenerator(STKStream* stream, STKGeneratorType type, StkFloat frequency, bool isRoot)
{
	STKNode* node = (STKNode*) STKGeneratorCreate(type, frequency);
	STKNodeSetStream(node, stream);
	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
		STKNodeSetIsRoot(node, true);
		stream->m_roots.push_back(node);
	}

	else STKNodeSetIsRoot(node, false);
		
	return node;
}

// ----------------------------------------------------------------------
//	STK ADD ENVELOPE NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddEnvelope(STKStream* stream, STKEnvelopeType type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*) STKEnvelopeCreate(type, source);
	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
		STKNodeSetIsRoot(node, true);
		stream->m_roots.push_back(node);
	}
	else STKNodeSetIsRoot(node, false);

	return node;
}

// ----------------------------------------------------------------------
//	STK ADD ARYTHMETIC NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddArythmetic(STKStream* stream, STKArythmeticMode mode, STKNode* lhs, STKNode* rhs, bool isRoot)
{
	STKNode* node = (STKNode*)STKArythmeticCreate(lhs, rhs, mode);
	if(lhs)STKNodeIncrementNumOutput(lhs);
	if(rhs)STKNodeIncrementNumOutput(rhs);

	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
	    STKNodeSetIsRoot(node, true);
	    stream->m_roots.push_back(node);
	}
	else STKNodeSetIsRoot(node, false);

	return node;
	
	return NULL;
}

// ----------------------------------------------------------------------
//	STK ADD EFFECT NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddEffect(STKStream* stream, STKEffectType type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)STKEffectCreate(source, type);
	
	STKNodeIncrementNumOutput(source);
	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
		STKNodeSetIsRoot(node, true);
		stream->m_roots.push_back(node);
	}
	else STKNodeSetIsRoot(node, false);
		
	return node;
}

// ----------------------------------------------------------------------
//	STK GENERATOR ADD FILTER NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddFilter(STKStream* stream, STKFilterType type, STKNode* source, bool isRoot)
{
	STKNode* node = (STKNode*)STKFilterCreate(source, type);

	STKNodeIncrementNumOutput(source);
	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
		STKNodeSetIsRoot(node, true);
		stream->m_roots.push_back(node);
	}
	else STKNodeSetIsRoot(node, false);

	return node;
}


// ----------------------------------------------------------------------
//	STK ADD BUFFER NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddBuffer(STKStream* stream, STKNode* previous, bool isRoot)
{
	STKNode* node = (STKNode*)STKBufferCreate(previous);
	STKNodeSetStream(node, stream);
	STKNodeIncrementNumOutput(node);
	if (isRoot)
	{
		STKNodeSetIsRoot(node, true);
		stream->m_roots.push_back(node);
	}

	else STKNodeSetIsRoot(node, false);

	return node;
}

// ----------------------------------------------------------------------
//    STK ADD READER NODE
// ----------------------------------------------------------------------
STKNode* STKStreamAddReader(STKStream* stream, const char* filename, bool isRoot)
{
    STKReader* reader = new STKReader();
    STKNode* node = static_cast<STKNode*>(reader);
    STKNodeSetStream(node, stream);
    STKNodeIncrementNumOutput(node);
    if (isRoot)
    {
        STKNodeSetIsRoot(node, true);
        stream->m_roots.push_back(node);
    }
    
    else STKNodeSetIsRoot(node, false);
    STKReaderSetFile(reader, filename);
    return node;
    
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
			*samples += STKNodeTick(stream->m_roots[j]) * weight;
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
