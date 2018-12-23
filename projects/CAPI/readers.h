
#ifndef STK_READER_H
#define STK_READER_H

#include "common.h"
#include "FileWvIn.h"
#include "FileRead.h"
#include "FileLoop.h"

class STKReader : public STKNode{
public:
	enum Mode {

	};

	enum Param {

	};

	StkFloat tick(unsigned int channel = 0);
	// constructor
	STKReader();
	// destructor 
	~STKReader();
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
	float m_frequency;
	FileWvIn m_file;
	FileRead m_read;
	std::string m_filename;
};

EXPORT void STKSetReaderMode(STKReader* reader, STKReader::Mode mode);
EXPORT void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar);

#endif