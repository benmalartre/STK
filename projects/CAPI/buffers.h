#ifndef STK_BUFFER_H
#define STK_BUFFER_H

#include "common.h"

#define CIRCULAR_BUFFER_PHASES 4
struct STKCircularBuffer{
	StkFloat* m_datas;
	int m_phase;
	int m_idx;
	int m_rate;
	int m_chunck;

	STKCircularBuffer(){
		m_datas = NULL;
		m_rate = std::ceil(Stk::sampleRate());
		m_idx = 0;
		m_phase = 0;
	}

	~STKCircularBuffer(){
		if (m_datas)delete[] m_datas;
	}

	void init(int rate){
		if (m_datas != NULL) delete[] m_datas;
		m_datas = new StkFloat[rate * CIRCULAR_BUFFER_PHASES];
		m_idx = 0;
		m_phase = 0;
	}

	void set(StkFloat value){
		m_idx++;
		if (m_idx >= m_rate){
			m_idx = 0;
			m_phase++;
			if (m_phase == CIRCULAR_BUFFER_PHASES)
			{
				m_phase = 0;
			}
		}
		m_datas[m_phase * m_rate + m_idx] = value;
	}

	void get(int phase, int offset, void* datas){
		memcpy(datas, &m_datas[0], 256 * sizeof(float));
	}
};

class STKBuffer : public STKNode{
public:
	
	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKBuffer(STKNode* previous);
	// destructor 
	~STKBuffer();
	// overrides
	void reset(){ m_outidx = 0; };
	void init();
	void term();
	// functions
	void setScalar(StkFloat scalar){ m_scalar = scalar; };
	void setHasNoEffect(bool hasnoeffect);
	void setPrevious(STKNode* node);
	STKNode* getPrevious(){ return m_source; };
	STKCircularBuffer* getBuffer(){ return &m_buffer; };

private:
	inline StkFloat BufferTick();
	inline StkFloat BufferTickHasNoEffect();
	std::function<StkFloat()> m_tickCallback;
	STKNode* m_source;
	StkFloat m_scalar;
	STKCircularBuffer  m_buffer;
};

EXPORT void STKSetBufferScalar(STKBuffer* buffer, StkFloat scalar);
EXPORT void STKSetBufferPrevious(STKBuffer* buffer, STKNode* node);
EXPORT void STKGetBufferSample(STKBuffer* buffer, StkFloat* datas);

#endif
