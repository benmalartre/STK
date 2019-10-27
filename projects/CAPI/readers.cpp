#include "readers.h"
#include "nodes.h"
//--------------------------------------------------------------------
// STKReader Node Constructor
//--------------------------------------------------------------------
STKReader* STKReaderCreate()
{
    STKReader* reader = new STKReader();
    reader->m_r.m_file = NULL;
    reader->m_r.m_loop = NULL;
	reader->m_noutput = 0;
	reader->m_volume = 1.0;
	reader->m_outidx = 0;
	reader->m_mode = READER_FILEWVIN;
	reader->m_filename = "";
    STKReaderInit(reader);
}

//--------------------------------------------------------------------
// STKReader Node Destructor
//--------------------------------------------------------------------
void STKReaderDelete(STKReader* reader)
{
    STKReaderTerm(reader);
    delete reader;
}

//--------------------------------------------------------------------
// STKReader Node Reset
//--------------------------------------------------------------------
void STKReaderReset(STKReader* reader){
    
    reader->m_outidx = 0;
    switch (reader->m_mode)
    {
        case READER_FILEWVIN:
        {
            reader->m_r.m_file->reset();
            break;
        }
            
        case READER_FILELOOP:
        {
            reader->m_r.m_loop->reset();
            break;
        }
    }
}

//--------------------------------------------------------------------
// STKReader Node Init
//--------------------------------------------------------------------
void STKReaderInit(STKReader* reader)
{
	switch (reader->m_mode)
	{
		case READER_FILEWVIN:
		{
			reader->m_r.m_file = new FileWvIn();
			reader->m_mode = READER_FILEWVIN;
			reader->m_tickCallback = [reader](){ return STKReaderTickFileWvIn(); };
			break;
		}

		case READER_FILELOOP:
		{
			reader->m_r.m_loop = new FileLoop();
			reader->m_mode = READER_FILELOOP;
			reader->m_tickCallback = [reader](){ return STKReaderTickFileLoop(); };
			break;
		}
	}
}

//--------------------------------------------------------------------
// STKReader Node Term
//--------------------------------------------------------------------
void STKReaderTerm(STKReader* reader)
{
    reader->m_tickCallback = [reader](){return STKReaderTickHasNoEffect(reader);};
    if(reader->m_mode == READER_FILEWVIN && reader->m_r.m_file){
        reader->m_r.m_file->closeFile();
        delete reader->m_r.m_file;
    }
    if(reader->m_mode == READER_FILELOOP && reader->m_r.m_loop){
        reader->m_r.m_loop->closeFile();
        delete reader->m_r.m_loop;
    }
}

//--------------------------------------------------------------------
// STKReader Functions
//--------------------------------------------------------------------
StkFloat STKReaderTick(STKReader* reader, unsigned int channel )
{
	return reader->m_tickCallback();
}

void STKReaderSetFile(STKReader* reader, const char* filename)
{
    reader->m_filename = std::string(filename);
	switch (reader->m_mode)
	{
		case READER_FILEWVIN:
		{
            if(reader->m_r.m_loop){
			    if (reader->m_r.m_loop->isOpen())
                    reader->m_r.m_loop->closeFile();
            }
            else if(reader->m_r.m_file){
                if (reader->m_r.m_file->isOpen())
                    reader->m_r.m_file->closeFile();
            }
			reader->m_r.m_file->openFile(reader->m_r.m_filename, false);
            StkFloat fileSampleRate = reader->m_r.m_file->getFileRate();
            if ( Stk::sampleRate() != fileSampleRate )reader->m_r.m_file->setRate( fileSampleRate/ Stk::sampleRate() );
            
			break;
		}

		case FILELOOP:
		{
            if(reader->m_r.m_file){
                if (reader->m_r.m_file->isOpen())
                    reader->m_r.m_file->closeFile();
            }
            else if(reader->m_r.m_loop){
                if (reader->m_r.m_loop->isOpen())
                    reader->m_r.m_loop->closeFile();
            }
			reader->m_r.m_loop->openFile(m_filename, false);
            StkFloat fileSampleRate = reader->m_r.m_loop->getFileRate();
            if ( Stk::sampleRate() != fileSampleRate)reader->m_r.m_loop->setRate( fileSampleRate / Stk::sampleRate() );
            
            break;
		}
	}
   
};

StkFloat STKReaderGetFileSampleRate(STKReader* reader){
    switch(reader->m_mode)
    {
        case READER_FILEWVIN:
        {
            if(reader->m_r.m_file && reader->m_r.m_file->isOpen())
                return reader->m_r.m_file->getFileRate();
            else return -1.0f;
            break;
        }
        case READER_FILELOOP:
        {
            
            if(reader->m_r.m_loop)
            {
                FileWvIn* file = (FileWvIn*)reader->m_r.m_loop;
                if(file->isOpen())return m_loop->getFileRate();
                else return -1.0f;
            }
        }
    }
    
}

void STKReaderSetScalar(STKReader* reader, STKReaderParam param, StkFloat scalar)
{
    switch(reader->m_mode)
    {
        case READER_FILEWVIN:
        {
            switch(param){
                case READER_RATE:
                {
                    reader->m_r.m_file->setRate(scalar);
                    break;
                }
                case READER_FREQUENCY:{
                    break;
                }
                case READER_ADD_TIME:
                {
                    reader->m_r.m_file->addTime(scalar);
                    break;
                }
                case READER_ADD_PHASE:
                {
                    break;
                }
                case READER_ADD_PHASE_OFFSET:
                {
                    break;
                }
            }
            break;
        }
        case READER_FILELOOP:
        {
            switch(param)
            {
                case READER_RATE:
                {
                    reader->m_r.m_loop->setRate(scalar);
                    break;
                }
                case READER_FREQUENCY:
                {
                    reader->m_r.m_loop->setFrequency(scalar);
                    break;
                }
                case READER_ADD_TIME:
                {
                    reader->m_r.m_loop->addTime(scalar);
                    break;
                }
                case READER_ADD_PHASE:
                {
                    reader->m_r.m_loop->addPhase(scalar);
                    break;
                }
                case READER_ADD_PHASE_OFFSET:
                {
                    reader->m_r.m_loop->addPhaseOffset(scalar);
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
void STKReaderSetHasNoEffect(STKReader* reader, bool hasnoeffect)
{
	if (hasnoeffect != reader->m_hasnoeffect)
	{
		if (hasnoeffect)
		{
			reader->m_tickCallback = [reader](){return STKReaderTickHasNoEffect(reader); };
		}
		else
		{
			STKReaderTerm(reader);
			STKReaderInit(reader);
		}
	}
}

StkFloat STKReaderTickHasNoEffect(STKReader* reader)
{
	return 0;
}

StkFloat STKReaderTickFileWvIn(STKReader* r)
{
	if (r->m_outidx == 0)
        return STKNodeUpdate((STKNode*)r, r->m_r.m_file->tick()*r->m_volume);
	else
        return STKNodeUpdate((STKNode*)r, r->m_r.m_file->lastOut());

}
StkFloat STKReaderTickFileLoop(STKReader* r)
{
    if (r->m_outidx == 0)
        return STKNodeUpdate((STKNode*)r, r->m_r.m_loop->tick()*r->m_volume);
    else
        return STKNodeUpdate((STKNode*)r, r->m_r.m_loop->lastOut());
}
//--------------------------------------------------------------------
// EXPORTED FUNCTIONS
//--------------------------------------------------------------------
void STKSetReaderMode(STKReader* reader, STKReaderMode mode)
{
    STKReaderTerm(reader);
    STKReaderSetMode(reader, mode);
    STKReaderInit(reader);
    STKReaderSetFile(reader, reader->m_filename.c_str());
}

void STKSetReaderScalar(STKReader* reader, STKReaderParam param, StkFloat scalar)
{
    STKReaderSetScalar(reader, param, scalar);
}

void STKSetReaderFilename(STKReader* reader, const char* filename)
{
    STKReaderTerm(reader);
    STKReaderInit(reader);

	STKReaderSetFile(reader, filename);
}

void STKResetReader(STKReader* reader){
    STKReaderReset(reader);
}

StkFloat STKGetReaderFileSampleRate(STKReader* reader){
    STKReaderGetFileSampleRate(reader);
}
