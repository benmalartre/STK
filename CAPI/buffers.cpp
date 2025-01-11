#include "buffers.h"
#include "exports.h"
//--------------------------------------------------------------------
// STKBuffer Node Constructor
//--------------------------------------------------------------------
STKBuffer* STKBufferCreate(STKNode* source)
{
  STKBuffer* b = new STKBuffer();
	b->m_volume = 1.0;
	b->m_source = source;
  b->m_type = STK_BUFFER;
  b->m_datas = NULL;
  b->m_rate = std::ceil(Stk::sampleRate());
  b->m_idx = 0;
  b->m_phase = 0;
  
  STKBufferInit(b, Stk::sampleRate());
  return b;
}

//--------------------------------------------------------------------
// STKBuffer Node Destructor
//--------------------------------------------------------------------
void STKBufferDelete(STKBuffer* b)
{
  if (b->m_datas)delete[] b->m_datas;
  STKBufferTerm(b);
  delete b;
}

//--------------------------------------------------------------------
// STKBuffer Set Has No Effect
//--------------------------------------------------------------------
void STKBufferSetHasNoEffect(STKBuffer* b,bool hasnoeffect)
{
	if (hasnoeffect != b->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			b->m_tickCallback = [b](){return STKBufferTickHasNoEffect(b); };
		}
		else
		{
			STKBufferTerm(b);
            STKBufferInit(b, Stk::sampleRate());
		}
	}
}


//--------------------------------------------------------------------
// STKBuffer Node Init
//--------------------------------------------------------------------
void STKBufferInit(STKBuffer* b, int rate)
{
    if (b->m_datas != NULL) delete[] b->m_datas;
    
    b->m_rate = rate;
    b->m_idx = 0;
    b->m_phase = 0;
    
    b->m_datas = new StkFloat[b->m_rate * BUFFER_PHASES];
	b->m_tickCallback = [b](){ return STKBufferTick(b); };
}


//--------------------------------------------------------------------
// STKBuffer Node Term
//--------------------------------------------------------------------
void STKBufferTerm(STKBuffer* b)
{
}


//--------------------------------------------------------------------
// STKBuffer Node Tick()
//--------------------------------------------------------------------
StkFloat STKBufferTick(STKBuffer* b)
{
	StkFloat value = STKNodeTick(b->m_source) * b->m_volume;
	return value;
}

StkFloat STKBufferTickHasNoEffect(STKBuffer* b)
{
	return STKNodeTick(b->m_source);
}

StkFloat STKBufferTick(STKBuffer* b, unsigned int channel)
{
	if (!b->m_source) return 0;
	return b->m_tickCallback();
}

void STKBufferSetPrevious(STKBuffer* b, STKNode* node){
	if (b->m_source != NULL)STKBufferTerm(b);
	b->m_source = node;
    STKBufferInit(b, Stk::sampleRate());
}

// ----------------------------------------------------------------------
//	STK GENERATOR ARYTHMETIC SETTER
// ----------------------------------------------------------------------
void STKSetBufferScalar(STKBuffer* b, StkFloat scalar)
{
    b->m_scalar = scalar;
}

void STKSetBufferSource(STKBuffer* b, STKNode* source)
{
	b->m_source = source;
}

void STKGetBufferSample(STKBuffer* b, StkFloat* datas){
	//STKBufferGet()->get(0, 0, datas);
}
