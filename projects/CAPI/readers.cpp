#include "readers.h"

//--------------------------------------------------------------------
// STKReader Node Constructor
//--------------------------------------------------------------------
STKReader::STKReader()
{
	m_noutput = 0;
	m_volume = 1.0;
	m_outidx = 0;
	m_mode = FILELOOP;
	m_filename = "";
	init();
}

//--------------------------------------------------------------------
// STKGenerator Node Terminate
//--------------------------------------------------------------------
void STKReader::term()
{
	
}

//--------------------------------------------------------------------
// STKGenerator Node Init
//--------------------------------------------------------------------
void STKReader::init()
{
	switch (m_mode)
	{
		case FILEWVIN:
		{
			m_file = new FileWvIn();
			m_mode = FILEWVIN;
			m_tickCallback = [this](){ return this->ReaderTickFileWvIn(); };
			break;
		}

		case FILELOOP:
		{
			m_loop = new FileLoop();
			m_mode = FILELOOP;
			m_tickCallback = [this](){ return this->ReaderTickFileLoop(); };
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKGenerator Node Reset
//--------------------------------------------------------------------
	void STKReader::reset(){

	m_outidx = 0;
	switch (m_mode)
	{
		case FILEWVIN:
		{
			m_file->reset();
			break;
		}

		case FILELOOP:
		{
			m_loop->reset();
			break;
		}
	}
}


//--------------------------------------------------------------------
// STKGenerator Node Destructor
//--------------------------------------------------------------------
STKReader::~STKReader()
{
	term();
}

StkFloat STKReader::tick(unsigned int channel )
{
	return 0;
}

// functions
void STKReader::setFile(std::string filename)
{
	switch (m_mode)
	{
		case FILEWVIN:
		{
			if (m_file->isOpen())m_file->closeFile();
			m_file->openFile(filename);
			break;
		}

		case FILELOOP:
		{
			m_loop->openFile(filename);
		}
	}
};
void STKReader::setScalar(Param param, StkFloat scalar)
{

}
//--------------------------------------------------------------------
// STKEffect Set Has No Effect
//--------------------------------------------------------------------
void STKReader::setHasNoEffect(bool hasnoeffect)
{
	if (hasnoeffect != m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			m_tickCallback = [this](){return this->ReaderTickHasNoEffect(); };
		}
		else
		{
			term();
			init();
		}
	}
}

StkFloat STKReader::ReaderTickHasNoEffect()
{
	return 0;
}

StkFloat STKReader::ReaderTickFileWvIn()
{
	if (m_outidx == 0)return update(m_file->tick()*m_volume);
	else return update(m_file->lastOut());
}
StkFloat STKReader::ReaderTickFileLoop()
{
	if (m_outidx == 0)return update(m_loop->tick()*m_volume);
	else return update(m_loop->lastOut());
}


void STKSetReaderMode(STKReader* reader, STKReader::Mode mode)
{

}

void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar)
{

}

void STKSetReaderFilename(STKReader* reader, const char* filename)
{
	reader->setFile((std::string)filename);
}