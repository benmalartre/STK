#include "writers.h"

StkFloat STKWriter::tick(unsigned int channel )
{
	return 0;
}

// constructor
STKWriter::STKWriter()
{

}

// destructor 
STKWriter::~STKWriter(){
}

// overrides
void STKWriter::reset(){};
void STKWriter::init(){};
void STKWriter::term(){};

// functions
void STKWriter::setFile(std::string filename)
{
};
void STKWriter::setScalar(Param param, StkFloat scalar)
{

}
void STKWriter::setHasNoEffect(bool hasnoeffect){};

/*
private:
	std::function<StkFloat()> m_tickCallback;
	float m_frequency;
	FileWvIn m_file;
	FileRead m_read;
	std::string m_filename;

};
*/
void STKSetWriterMode(STKWriter* reader, STKWriter::Mode mode)
{

}

void STKSetWriterScalar(STKWriter* reader, STKWriter::Param param, StkFloat scalar)
{

}
