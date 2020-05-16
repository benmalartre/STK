
#pragma once

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


// constructor
STKWriter* STKWriterCreate();

// destructor
void STKWriterDelete(STKWriter* writer);

// functions
void STKWriterReset(STKWriter* writer);
void STKWriterInit(STKWriter* writer);
void STKWriterTerm(STKWriter* writer);


// tick functions
StkFloat STKWriterTick(STKWriter* w, unsigned int channel = 0);

// exported functions
<<<<<<< HEAD
EXPORT void STKWriterSetFile(STKWriter* writer, std::string filename);
EXPORT void STKWriterSetHasNoEffect(STKWriter* writer, bool hasnoeffect);;

#endif
=======
EXPORT void STKSetWriterMode(STKWriter* writer, STKWriterMode mode);
EXPORT void STKSetWriterScalar(STKWriter* writer, STKWriterParam param, StkFloat scalar);
>>>>>>> 4a0e8e95b7df1f6569c1ace8a20d77850600d13e
