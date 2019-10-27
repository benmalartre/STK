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

	STKCircularBuffer* STKCircularBufferCreate(){
		STKCircularBuffer* b = new STKCircularBuffer();
		b->m_datas = NULL;
		b->m_rate = std::ceil(Stk::sampleRate());
		b->m_idx = 0;
		b->m_phase = 0;
		return b;
	}

	STKCircularBufferDelete(STKCircularBuffer* b){
		if (b->m_datas)delete[] b->m_datas;
		delete b;
	}

	void STKBufferInit(STKCircularBuffer*b, int rate){
		if (b->m_datas != NULL) delete[] b->m_datas;
		b->m_datas = new StkFloat[rate * CIRCULAR_BUFFER_PHASES];
		b->m_rate = rate;
		b->m_idx = 0;
		b->m_phase = 0;
	}

	void STKBufferSet(STKCircularBuffer* b, StkFloat value){
		b->m_idx++;
		if (b->m_idx >= b->m_rate){
			b->m_idx = 0;
			b->m_phase++;
			if (b->m_phase == CIRCULAR_BUFFER_PHASES)
			{
				b->m_phase = 0;
			}
		}
		b->m_datas[b->m_phase * b->m_rate + b->m_idx] = value;
	}

	void STKBufferGet(STKCircularBuffer* b, int phase, int offset, void* datas){
		memcpy(datas, &b->m_datas[0], 256 * sizeof(float));
	}
};

struct STKBuffer : public STKNode{
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
