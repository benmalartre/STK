
#ifndef STK_WRITER_H
#define STK_WRITER_H

#include "common.h"
#include "FileWvOut.h"
#include <cstdlib>

class STKWriter : public STKNode{
public:
	enum Format {
        RAW,
        WAVE,
        SND,
        AIFF,
        MAT
	};

	enum Param {

	};

	
	// constructor
	STKWriter();
	// destructor 
	~STKWriter();
	// overrides
	void reset();
	void init();
	void term();
	// functions
    StkFloat tick(unsigned int channel = 0);
	void setFile(const char* filename);
	void setHasNoEffect(bool hasnoeffect);

private:
	std::function<StkFloat()> m_tickCallback;
    int m_format;
	FileWvOut m_file;
	std::string m_filename;
};

EXPORT void STKSetWriterFormat(STKWriter* writer, STKWriter::Format format);
EXPORT void STKSetWriterScalar(STKWriter* writer, STKWriter::Param param, StkFloat scalar);

#endif
