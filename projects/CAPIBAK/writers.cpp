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
void STKWriter::setFile(const char* filename)
{
};

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
void STKSetWriterFormat(STKWriter* reader, STKWriter::Format format)
{

}

void STKSetWriterScalar(STKWriter* reader, STKWriter::Param param, StkFloat scalar)
{

}
