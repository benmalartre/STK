#ifndef STK_CAPI_READER_H
#define STK_CAPI_READER_H
#pragma once

#include "common.h"
#include "nodes.h"
#include "FileRead.h"
#include "FileWvIn.h"
#include "FileLoop.h"

typedef enum STKReaderMode {
    READER_FILEWVIN,
    READER_FILELOOP
}STKReaderMode;

typedef enum STKReaderParam {
    READER_RATE,
    READER_FREQUENCY,
    READER_ADD_TIME,
    READER_ADD_PHASE,
    READER_ADD_PHASE_OFFSET
}STKREADERParam;

typedef union STKReaderMOD{
    stk::FileLoop* m_loop;
    stk::FileWvIn* m_file;
}STKReaderMOD;

struct STKReader : public STKNode{
    float m_frequency;
    STKReaderMode m_mode;
    STKReaderMOD m_r;
    std::string m_filename;
};

// constructor
STKReader* STKReaderCreate();
// destructor
void STKReaderDelete(STKReader* reader);

// overrides
void STKReaderReset(STKReader* reader);
void STKReaderInit(STKReader* reader);
void STKReaderTerm(STKReader* reader);
 
// functions
inline StkFloat STKReaderTickFileWvIn(STKReader* reader);
inline StkFloat STKReaderTickFileLoop(STKReader* reader);
inline StkFloat STKReaderTickHasNoEffect(STKReader* reader);
StkFloat STKReaderTick(STKReader* reader, unsigned int channel = 0);
 
void STKReaderSetFile(STKReader* reader, const char* filename);
void STKReaderSetScalar(STKReader* reader, STKReaderParam param, StkFloat scalar);
void STKReaderSetHasNoEffect(STKReader* reader, bool hasnoeffect);
inline void STKReaderSetMode(STKReader* reader, STKReaderMode mode){reader->m_mode = mode;};
inline STKReaderMode STKReaderGetMode(STKReader* reader){return reader->m_mode;};
inline const char* STKReaderGetFileName(STKReader* reader){return reader->m_filename.c_str();};
StkFloat STKReaderGetFileSampleRate(STKReader* reader);

EXPORT void STKSetReaderMode(STKReader* reader, STKReaderMode mode);
EXPORT void STKSetReaderScalar(STKReader* reader, STKReaderParam param, StkFloat scalar);
EXPORT void STKSetReaderFilename(STKReader* reader, const char* filename);
EXPORT void STKResetReader(STKReader* reader);
EXPORT StkFloat STKGetReaderFileSampleRate(STKReader* reader);

#endif // STK_CAPI_READER_H
