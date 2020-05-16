
#ifndef STK_EXPORT_H
#define STK_EXPORT_H

#include "common.h"
#include "nodes.h"
#include "streams.h"
#include "nodes.h"
#include "generators.h"
#include "readers.h"
//#include "instruments.h"
#include "writers.h"

// exported functions
EXPORT RtAudio* STKInit();
EXPORT void STKTerm(RtAudio* DAC);
EXPORT void STKSetSampleRate(float rate);
EXPORT int STKGetDevices(RtAudio* DAC);

//EXPORT StkFloat STKNodeTick(STKNode*);
EXPORT void STKNodeSetHasNoEffect(STKNode* node, bool hasnoeffect);
EXPORT void STKNodeReset(STKNode*);
EXPORT void STKNodeInit(STKNode*);
EXPORT void STKNodeTerm(STKNode*);



/*
class STKStream;
// base class
class STKNode{
public:
	virtual StkFloat tick(unsigned int channel = 0) = 0;
	virtual void reset() = 0;
	virtual void init() = 0;
	virtual void term() = 0;
	virtual void setHasNoEffect(bool hasnoeffect) = 0;
	void incrementNumOutput(){ m_noutput++; };
	void decrementNumOutput(){ m_noutput--; };
	void setIsRoot(bool isRoot) { m_isroot = isRoot; };
	bool isRoot(){ return m_isroot; };
	void setStream(STKStream* stream) { m_stream = stream; };
	void setVolume(StkFloat volume){ m_volume = volume; };
	STKStream* getStream(){ return m_stream; };
protected:
    StkFloat update(StkFloat value);
	bool m_hasnoeffect;
	bool m_isroot;
	int m_noutput;
	int m_outidx;
	StkFloat m_volume;
	STKStream* m_stream;
    STKNodeType m_type;

};
*/

#endif
