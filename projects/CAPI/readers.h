
#ifndef STK_READER_H
#define STK_READER_H

#include "common.h"
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

union STKReaderMOD{
    FileLoop* m_loop;
    FileWvIn* m_file;
};

struct STKNode;
struct STKReader : public STKNode{

	std::function<StkFloat()> m_tickCallback;
	float m_frequency;
	Mode m_mode;
    STKReaderMOD m_reader;
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
void STKReaderSetScalar(STKReader* reader, Param param, StkFloat scalar);
void STKReaderSetHasNoEffect(STKReader* reader, bool hasnoeffect);
void STKReaderSetMode(STKReader* reader, STKReaderMode mode){m_mode = mode;};
STKReaderMode STKReaderGetMode(STKReader* reader){return m_mode;};
const char* STKReaderGetFileName(STKReader* reader){return m_filename.c_str();};
StkFloat STKReaderGetFileSampleRate(STKReader* reader);

EXPORT void STKSetReaderMode(STKReader* reader, STKReader::Mode mode);
EXPORT void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar);
EXPORT void STKSetReaderFilename(STKReader* reader, const char* filename);
EXPORT void STKResetReader(STKReader* reader);
EXPORT StkFloat STKGetReaderFileSampleRate(STKReader* reader);
#endif
