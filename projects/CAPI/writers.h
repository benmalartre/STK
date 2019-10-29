
#ifndef STK_WRITER_H
#define STK_WRITER_H

#include "common.h"
#include "nodes.h"
#include "FileWvOut.h"
#include <cstdlib>


typedef enum STKWriterMode {
    
}STKWriterMode;

typedef enum STKWriterParam {
    
}STKWriterParam;

struct STKWriter : public STKNode{
	std::function<StkFloat()> m_tickCallback;
	FileWvOut m_file;
	std::string m_filename;
};

// functions

StkFloat STKWriterTick(unsigned int channel = 0);
// constructor
STKWriter* STKWriterCreate();

// destructor
void STKWriterDelete(STKWriter* writer);

// functions
void STKWriterReset(STKWriter* writer);
void STKWriterInit(STKWriter* writer);
void STKWriterTerm(STKWriter* writer);
void STKWriterSetFile(STKWriter* writer, std::string filename);
void STKWriterSetHasNoEffect(STKWriter* writer, bool hasnoeffect);

// exported functions
EXPORT void STKSetWriterMode(STKWriter* writer, STKWriterMode mode);
EXPORT void STKSetWriterScalar(STKWriter* writer, STKWriterParam param, StkFloat scalar);

#endif
