#include "buffers.h"

//--------------------------------------------------------------------
// STKBuffer Node Constructor
//--------------------------------------------------------------------
STKBuffer::STKBuffer(STKNode* source)
{
	m_volume = 1.0;
	m_source = source;
	init();
}

//--------------------------------------------------------------------
// STKBuffer Set Has No Effect
//--------------------------------------------------------------------
void STKBuffer::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->BufferTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}


//--------------------------------------------------------------------
// STKBuffer Node Init
//--------------------------------------------------------------------
void STKBuffer::init()
{
	m_buffer.init(Stk::sampleRate());
	m_tickCallback = [this](){ return this->BufferTick(); };
}


//--------------------------------------------------------------------
// STKBuffer Node Term
//--------------------------------------------------------------------
void STKBuffer::term()
{
}


//--------------------------------------------------------------------
// STKBuffer Node Tick()
//--------------------------------------------------------------------
StkFloat STKBuffer::BufferTick()
{
	StkFloat value = m_source->tick() * m_volume;

	return value;
}

StkFloat STKBuffer::BufferTickHasNoEffect()
{
	return m_source->tick();
}

StkFloat STKBuffer::tick(unsigned int channel)
{
	if (!m_source) return 0;
	return m_tickCallback();
}

void STKBuffer::setPrevious(STKNode* node){
	if (m_source != NULL)term();
	m_source = node;
	init();
}

// ----------------------------------------------------------------------
//	STK GENERATOR ARYTHMETIC SETTER
// ----------------------------------------------------------------------
void STKSetBufferScalar(STKBuffer* buffer, StkFloat scalar)
{
	buffer->setScalar(scalar);
}

void STKSetBufferPrevious(STKBuffer* buffer, STKNode* node)
{
	buffer->setPrevious(node);
}

void STKGetBufferSample(STKBuffer* buffer, StkFloat* datas){
	buffer->getBuffer()->get(0, 0, datas);
}