#include <iostream>
#include <SineWave.h>
#include <Blit.h>
#include <Bowed.h>
#include <Twang.h>
#include <RtAudio.h>
#include "sequencer.h"
using namespace stk;

#define FRAMES_BLOCK_SIZE 1024


struct TickData {
  Sequencer *sequencer;
  Twang *twang;
  Blit *blit;
  SineWave *frequency;
  SineWave *generator;
  SineWave *lfo;
  StkFloat base_frequency;
  StkFloat scaler;
  long counter;
  short num_channels;
  bool done;

  // Default constructor.
  TickData()
    : generator(NULL), lfo(NULL), scaler(1.0), counter(0), done( false ) {}
};

static float PositiveSineWaveTick(SineWave* wave, float multiplier)
{
  return (wave->tick() + 1) * 0.5+ 0.2;
}

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  TickData *data = (TickData *) userData;
  StkFloat *samples = (StkFloat *) outputBuffer;
  Sequencer *sequencer = data->sequencer;
  uint64_t timeIdx = sequencer->TimeToIndex(streamTime);
  std::cout << "timeIdx " << timeIdx << std::endl;
  int time = sequencer->Get(0, timeIdx);
  data->generator->setFrequency(time);
  for ( unsigned int i=0; i<nBufferFrames; i++ ) {
    
    float sample = data->generator->tick() + data->lfo->tick() * 0.25f;
    for(int i=0; i < data->num_channels; ++i) {
      *samples++ = sample;
    }
    /*
    data->generator->addPhaseOffset(
      data->frequency->tick());
    float sample = 
      data->generator->tick() + data->lfo->tick() * 0.25f;
    if(int(streamTime) % 2 < 1)sample += data->blit->tick();
    else sample += data->twang->tick(120);
    for(int i=0; i < data->num_channels; ++i) {
      *samples++ = sample;
    }
    */
  }

  if ( data->counter > 80000 )
    data->done = true; 

  return 0;
}


int main()
{
  
  // Set the global sample rate and rawwave path before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::setRawwavePath( "../../rawwaves/" );

  TickData data;
  RtAudio dac;

  // Figure out how many bytes in an StkFloat and setup the RtAudio stream.
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 2;
  data.num_channels = parameters.nChannels;
  RtAudioFormat format = 
    ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  unsigned int bufferFrames = RT_BUFFER_SIZE;
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), 
    &bufferFrames, &tick, (void *)&data );
  }
  catch ( RtAudioError& error ) {
    error.printMessage();
    goto cleanup;
  }

  try {
    // initialize tick data
    data.sequencer = new Sequencer;
    data.sequencer->SetLength(16);
    data.sequencer->AddTrack();
    for (size_t i = 0; i < 16; ++i) {
      data.sequencer->SetTime(0, i, Sequencer::Time(rand()));
    }
    
    data.generator = new SineWave();
    data.lfo = new SineWave();
    data.frequency = new SineWave();
    data.blit = new Blit();
    data.twang = new Twang();
  }
  catch ( StkError & ) {
    goto cleanup;
  }

  data.base_frequency = 96;
  data.lfo->setFrequency(0.25);
  data.frequency->setFrequency(1);
  data.generator->setFrequency(data.base_frequency);
  data.blit->setHarmonics(7);
  data.sequencer->Start();


  try {
    dac.startStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
    goto cleanup;
  }

  // Block waiting until callback signals done.
  while ( !data.done )
    Stk::sleep( 100 );
  
  // Shut down the callback and output stream.
  try {
    dac.closeStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
  }

 cleanup:
  delete data.generator;
  delete data.lfo;
  delete data.frequency;
  delete data.blit;
  delete data.twang;

  return 0;
}