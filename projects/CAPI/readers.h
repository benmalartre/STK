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
    FileLoop* m_loop;
    FileWvIn* m_file;
}STKReaderMOD;

struct STKNode;
typedef struct STKReader : public STKNode{

	std::function<StkFloat()> m_tickCallback;
	float m_frequency;
	STKReaderMode m_mode;
    STKReaderMOD m_r;
	std::string m_filename;
}STKReader;

// constructor
EXPORT STKReader* STKReaderCreate();
// destructor
static void STKReaderDelete(STKReader* reader);

// overrides
static void STKReaderReset(STKReader* reader);
static void STKReaderInit(STKReader* reader);
static void STKReaderTerm(STKReader* reader);
 
// functions
static inline StkFloat STKReaderTickFileWvIn(STKReader* reader);
static inline StkFloat STKReaderTickFileLoop(STKReader* reader);
static inline StkFloat STKReaderTickHasNoEffect(STKReader* reader);
EXPORT StkFloat STKReaderTick(STKReader* reader, unsigned int channel = 0);
 
EXPORT void STKReaderSetFile(STKReader* reader, const char* filename);
static void STKReaderSetScalar(STKReader* reader, STKReaderParam param, StkFloat scalar);
static void STKReaderSetHasNoEffect(STKReader* reader, bool hasnoeffect);
static void STKReaderSetMode(STKReader* reader, STKReaderMode mode){reader->m_mode = mode;};
static STKReaderMode STKReaderGetMode(STKReader* reader){return reader->m_mode;};
static const char* STKReaderGetFileName(STKReader* reader){return reader->m_filename.c_str();};
static StkFloat STKReaderGetFileSampleRate(STKReader* reader);

EXPORT void STKSetReaderMode(STKReader* reader, STKReaderMode mode);
EXPORT void STKSetReaderScalar(STKReader* reader, STKReaderParam param, StkFloat scalar);
EXPORT void STKSetReaderFilename(STKReader* reader, const char* filename);
EXPORT void STKResetReader(STKReader* reader);
EXPORT StkFloat STKGetReaderFileSampleRate(STKReader* reader);
