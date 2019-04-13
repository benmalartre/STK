
#ifndef STK_READER_H
#define STK_READER_H

#include "common.h"
#include "FileRead.h"
#include "FileWvIn.h"
#include "FileLoop.h"

class STKReader : public STKNode{
public:
	enum Mode {
		FILEWVIN,
		FILELOOP
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
	Mode m_mode;
	union{
		FileLoop* m_loop;
		FileWvIn* m_file;
	};
	std::string m_filename;

	inline StkFloat ReaderTickFileWvIn();
	inline StkFloat ReaderTickFileLoop();
	inline StkFloat ReaderTickHasNoEffect();
};

EXPORT void STKSetReaderMode(STKReader* reader, STKReader::Mode mode);
EXPORT void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar);
EXPORT void STKSetReaderFilename(STKReader* reader, const char* filename);

#endif