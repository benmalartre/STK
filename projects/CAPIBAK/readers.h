
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
        RATE,
        FREQUENCY,
        ADD_TIME,
        ADD_PHASE,
        ADD_PHASE_OFFSET
	};
    
	// constructor
	STKReader();
	// destructor 
	~STKReader();
    
	// overrides
	void reset();
	void init();
	void term();
    
	// functions
    StkFloat tick(unsigned int channel = 0);
	void setFile(const char* filename);
	void setScalar(Param param, StkFloat scalar);
	void setHasNoEffect(bool hasnoeffect);
    void setMode(Mode mode){m_mode = mode;};
    Mode getMode(){return m_mode;};
    const char* getFileName(){return m_filename.c_str();};
    StkFloat getFileSampleRate();
    
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
EXPORT void STKResetReader(STKReader* reader);
EXPORT StkFloat STKGetReaderFileSampleRate(STKReader* reader);
#endif
