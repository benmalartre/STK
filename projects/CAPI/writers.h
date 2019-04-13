
#ifndef STK_WRITER_H
#define STK_WRITER_H

#include "common.h"
#include "FileWvOut.h"
#include <cstdlib>

class STKWriter : public STKNode{
public:
	enum Mode {

	};

	enum Param {

	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKWriter();
	// destructor 
	~STKWriter();
	// overrides
	void reset();
	void init();
	void term();
	// functions
	void setFile(std::string filename);
	void setScalar(Param param, StkFloat scalar);
	void setHasNoEffect(bool hasnoeffect);

private:
	std::function<StkFloat()> m_tickCallback;
	FileWvOut m_file;
	std::string m_filename;
};

EXPORT void STKSetWriterMode(STKWriter* writer, STKWriter::Mode mode);
EXPORT void STKSetWriterScalar(STKWriter* writer, STKWriter::Param param, StkFloat scalar);

#endif