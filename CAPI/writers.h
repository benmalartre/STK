
#ifndef STK_CAPI_WRITER_H
#define STK_CAPI_WRITER_H
#pragma once

#include "common.h"
#include "nodes.h"
#include "FileWvOut.h"
#include <cstdlib>


typedef enum STKWriterMode {
    
} STKWriterMode;

typedef enum STKWriterParam {
    
} STKWriterParam;

struct STKWriter : public STKNode{
    FileWvOut m_file;
    std::string m_filename;
};

// constructor
STKWriter* STKWriterCreate();

// destructor
void STKWriterDelete(STKWriter* writer);

// functions
void STKWriterReset(STKWriter* writer);
void STKWriterInit(STKWriter* writer);
void STKWriterTerm(STKWriter* writer);


// tick functions
EXPORT StkFloat STKWriterTick(STKWriter* w, unsigned int channel = 0);

// exported functions
EXPORT void STKWriterSetFile(STKWriter* writer, std::string filename);
EXPORT void STKWriterSetHasNoEffect(STKWriter* writer, bool hasnoeffect);;

#endif