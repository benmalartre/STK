#include <iostream>
#include "utils.h"
#include <Stk.h>
#include <FileRead.h>
#include <FileWvOut.h>

static const std::string raw_ext(".raw");
static const std::string wav_ext(".wav");
static const std::string snd_ext(".snd");
static const std::string aif_ext(".aiff");
static const std::string mat_ext(".mat");

#define FRAMES_BLOCK_SIZE 1024

int main (int argc, char *argv[])
{
  std::string source, destination;
  if(argc != 3) {
    std::cerr << "need source and destination full path name, conversion fail!" << std::endl;
    return -1;
  }
  source = argv[1];
  destination = argv[2];
  stk::FileRead read;

  if(!source.empty() && !destination.empty()) {
    // check output filename
    if(!EndsWithString(destination, wav_ext)) {
      std::cerr << "destination file is not '.wav', conversion fail!" << 
        std::endl;
      return -1;
    }

    // open input filename
    std::string extension = GetExtension(source);
    if(extension == raw_ext) {
      read.open( source, true);
    } else {
      read.open( source, false, 2, stk::FileRead::STK_SINT16, 22050.0);
    }
  }
  
  if(read.isOpen()) {
    unsigned long num_frames = read.fileSize();
    unsigned int num_channels = read.channels();
    stk::FileRead::StkFormat format = read.format();
    stk::StkFloat rate = read.fileRate();

    stk::FileWvOut write( destination, num_channels, stk::FileWrite::FILE_WAV, 
      format, FRAMES_BLOCK_SIZE );

    size_t counter = 0;
    stk::StkFrames frames(FRAMES_BLOCK_SIZE, num_channels);

    while(counter < num_frames) {
      read.read(frames, counter);
      counter += FRAMES_BLOCK_SIZE;
      write.tick(frames);
    }
    read.close();
    write.closeFile();

  } else {
    std::cerr << "[STK] fail open " << source << " source file, conversion fail" << std::endl;
  }

  return 0;	
}