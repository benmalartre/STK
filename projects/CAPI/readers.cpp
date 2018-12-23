#include "readers.h"

StkFloat STKReader::tick(unsigned int channel )
{
	return 0;
}

// constructor
STKReader::STKReader()
{

}

// destructor 
STKReader::~STKReader(){
}

// overrides
void STKReader::reset(){};
void STKReader::init(){};
void STKReader::term(){};

// functions
void STKReader::setFile(std::string filename)
{
};
void STKReader::setScalar(Param param, StkFloat scalar)
{

}
void STKReader::setHasNoEffect(bool hasnoeffect){};

/*
private:
	std::function<StkFloat()> m_tickCallback;
	float m_frequency;
	FileWvIn m_file;
	FileRead m_read;
	std::string m_filename;

};
*/
void STKSetReaderMode(STKReader* reader, STKReader::Mode mode)
{

}

void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar)
{

}
