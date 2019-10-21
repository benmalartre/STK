#include "readers.h"

//--------------------------------------------------------------------
// STKReader Node Constructor
//--------------------------------------------------------------------
STKReader::STKReader()
{
    m_file = NULL;
    m_loop = NULL;
	m_noutput = 0;
	m_volume = 1.0;
	m_outidx = 0;
	m_mode = FILEWVIN;
	m_filename = "";
    init();
}

//--------------------------------------------------------------------
// STKReader Node Destructor
//--------------------------------------------------------------------
STKReader::~STKReader()
{
    term();
}

//--------------------------------------------------------------------
// STKReader Node Reset
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
// STKReader Node Init
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
// STKReader Node Term
//--------------------------------------------------------------------
void STKReader::term()
{
    m_tickCallback = [this](){return this->ReaderTickHasNoEffect();};
    if(m_mode == FILEWVIN && m_file){
        m_file->closeFile();
        delete m_file;
    }
    if(m_mode == FILELOOP && m_loop){
        m_loop->closeFile();
        delete m_loop;
    }
}

//--------------------------------------------------------------------
// STKReader Functions
//--------------------------------------------------------------------
StkFloat STKReader::tick(unsigned int channel )
{
	return m_tickCallback();
}

void STKReader::setFile(const char* filename)
{
    m_filename = std::string(filename);
	switch (m_mode)
	{
		case FILEWVIN:
		{
			if (m_file->isOpen())m_file->closeFile();
			m_file->openFile(m_filename, false);
            StkFloat fileSampleRate = m_file->getFileRate();
            if ( Stk::sampleRate() != fileSampleRate )m_file->setRate( fileSampleRate/ Stk::sampleRate() );
            
			break;
		}

		case FILELOOP:
		{
            if (m_file->isOpen())m_file->closeFile();
			m_loop->openFile(m_filename, false);
            StkFloat fileSampleRate = m_loop->getFileRate();
            if ( Stk::sampleRate() != fileSampleRate)m_loop->setRate( fileSampleRate / Stk::sampleRate() );
            
            break;
		}
	}
   
};

StkFloat STKReader::getFileSampleRate(){
    switch(m_mode)
    {
        case FILEWVIN:
        {
            if(m_file && m_file->isOpen())
                return m_file->getFileRate();
            else return -1.0f;
            break;
        }
        case FILELOOP:
        {
            FileWvIn* file = (FileWvIn*)m_loop;
            if(m_loop && file->isOpen())
                return m_loop->getFileRate();
            else return -1.0f;
        }
    }
    
}

void STKReader::setScalar(Param param, StkFloat scalar)
{
    switch(m_mode)
    {
        case FILEWVIN:
        {
            switch(param){
                case RATE:
                {
                    m_file->setRate(scalar);
                    break;
                }
                case FREQUENCY:{
                    break;
                }
                case STKReader::ADD_TIME:
                {
                    m_file->addTime(scalar);
                    break;
                }
                case STKReader::ADD_PHASE:
                {
                    break;
                }
                case STKReader::ADD_PHASE_OFFSET:
                {
                    break;
                }
            }
            break;
        }
        case STKReader::FILELOOP:
        {
            switch(param)
            {
                case STKReader::RATE:
                {
                    m_loop->setRate(scalar);
                    break;
                }
                case STKReader::FREQUENCY:
                {
                    m_loop->setFrequency(scalar);
                    break;
                }
                case STKReader::ADD_TIME:
                {
                    m_loop->addTime(scalar);
                    break;
                }
                case STKReader::ADD_PHASE:
                {
                    m_loop->addPhase(scalar);
                    break;
                }
                case STKReader::ADD_PHASE_OFFSET:
                {
                    m_loop->addPhaseOffset(scalar);
                    break;
                }
            }
            break;
        }
    }
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
//--------------------------------------------------------------------
// EXPORTED FUNCTIONS
//--------------------------------------------------------------------
void STKSetReaderMode(STKReader* reader, STKReader::Mode mode)
{
    reader->term();
    reader->setMode(mode);
    reader->init();
    reader->setFile(reader->getFileName());
}

void STKSetReaderScalar(STKReader* reader, STKReader::Param param, StkFloat scalar)
{
    reader->setScalar(param, scalar);
}

void STKSetReaderFilename(STKReader* reader, const char* filename)
{
    reader->term();
    reader->init();

	reader->setFile(filename);
}

void STKResetReader(STKReader* reader){
    reader->reset();
}

StkFloat STKGetReaderFileSampleRate(STKReader* reader){
    return reader->getFileSampleRate();
}
