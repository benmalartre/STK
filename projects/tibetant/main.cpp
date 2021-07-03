#include <iostream>
#include <SineWave.h>
#include <RtAudio.h>
using namespace stk;

#define FRAMES_BLOCK_SIZE 1024


struct HHNode {
  std::vector<STKNode*> _childrens;

};

struct HHStream {
  short num_channels;
  RtAudio dac;

};

struct TickData {
  float frequency[6];
  SineWave *generator[5];
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
  register StkFloat *samples = (StkFloat *) outputBuffer;


  for ( unsigned int i=0; i<nBufferFrames; i++ ) {
    data->generator->addPhaseOffset(
      data->frequency->tick());
    float sample = 
      data->generator->tick() + data->lfo->tick() * 0.25f;
    for(int i=0; i < data->num_channels; ++i) {
      *samples++ = sample;
    }
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
    // Define and load the BeeThree instrument
    data.generator = new SineWave();
    data.lfo = new SineWave();
    data.frequency = new SineWave();
  }
  catch ( StkError & ) {
    goto cleanup;
  }

  data.base_frequency = 96;
  data.lfo->setFrequency(0.25);
  data.frequency->setFrequency(1);
  data.generator->setFrequency(data.base_frequency);


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

  return 0;
}