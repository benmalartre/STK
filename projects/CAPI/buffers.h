#pragma once

#include "common.h"
#include "nodes.h"
#define BUFFER_PHASES 4

/*
void STKCircularBufferSet(STKCircularBuffer* b, StkFloat value){
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

void STKCircularBufferGet(STKCircularBuffer* b, int phase, int offset, void* datas){
    memcpy(datas, &b->m_datas[0], 256 * sizeof(float));
}
*/

typedef struct STKBuffer : public STKNode {
  std::function<StkFloat()> m_tickCallback;
  STKNode* m_source;
  StkFloat m_scalar;

  StkFloat* m_datas;
  int m_phase;
  int m_idx;
  int m_rate;
  int m_chunck;
} STKBuffer;

// constructor
EXPORT STKBuffer* STKBufferCreate(STKNode* previous);

// destructor
static void STKBufferDelete(STKBuffer* b);

// overrides
static void STKBufferReset(STKBuffer* b){ b->m_outidx = 0; };
static void STKBufferInit(STKBuffer* b, int rate);
static void STKBufferTerm(STKBuffer* b);

// functions
static void STKBufferSetScalar(STKBuffer* b, StkFloat scalar){ b->m_scalar = scalar; };
static void STKBufferSetHasNoEffect(STKBuffer* b, bool hasnoeffect);
static void STKBufferSetSource(STKBuffer* b, STKNode* node);
static STKNode* STKBufferGetSource(STKBuffer* b){ return b->m_source; };
static StkFloat STKBufferGet(STKBuffer* b);
static void STKBufferSet(STKBuffer* b, StkFloat v);
static inline StkFloat STKBufferTickBuffer(STKBuffer* b);
static inline StkFloat STKBufferTickHasNoEffect(STKBuffer* b);
static StkFloat STKBufferTick(STKBuffer* b, unsigned int channel = 0);

EXPORT void STKSetBufferScalar(STKBuffer* buffer, StkFloat scalar);
EXPORT void STKSetBufferPrevious(STKBuffer* buffer, STKNode* node);
EXPORT void STKGetBufferSample(STKBuffer* buffer, StkFloat* datas);
