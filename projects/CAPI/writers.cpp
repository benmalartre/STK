#include "writers.h"

// constructor
STKWriter* STKWriterCreate()
{
    STKWriter* writer = new STKWriter();
    writer->m_type = STK_WRITER;
    return writer;
}

// destructor
void STKWriterDelete(STKWriter* writer){
    delete writer;
}

StkFloat STKWriterTick(STKWriter* writer, unsigned int channel )
{
	return 0;
}

// functions
void STKWriterReset(STKWriter* writer){};
void STKWriterInit(STKWriter* writer){};
void STKWriterTerm(STKWriter* writer){};
void STKWriterSetFile(STKWriter* writer, std::string filename)
{
};

void STKWriterSetHasNoEffect(STKWriter* writer, bool hasnoeffect){};

void STKSetWriterMode(STKWriter* writer, STKWriterMode mode)
{

}

void STKSetWriterScalar(STKWriter* writer, STKWriterParam param, StkFloat scalar)
{

}
