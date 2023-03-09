#include <GLFW/glfw3.h>
#include <iostream>
#include <SineWave.h>
#include <Blit.h>
#include <Bowed.h>
#include <Plucked.h>
#include <RtAudio.h>
#include "generator.h"
#include "sequencer.h"
using namespace stk;

#define FRAMES_BLOCK_SIZE 1024



struct TickData {
  stk::StkFrames  frames;
  Sequencer       sequencer;
  WaveGenerator   generator;
  size_t          waveFormIdx;
  StkFloat        base_frequency;
  StkFloat        scaler;
  long            counter;
  short           num_channels;
  bool            done;

  // Default constructor.
  TickData()
    : scaler(1.0), counter(0), done( false ) {}
};


int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  TickData *data = (TickData *) userData;
  StkFloat *samples = (StkFloat *) outputBuffer;
  Sequencer *sequencer = &data->sequencer;
  uint64_t timeIdx = sequencer->timeToIndex(streamTime);

  Sequencer::Track* track = sequencer->getTrack(0);
  if(track) {
    //track->setWaveForm(data->waveFormIdx++);
    track->noteOn(timeIdx);
  }


  data->counter++;
  if(data->counter > 8) {
    data->counter = 0;
    
  }

  sequencer->tick(data->frames);
  //stk::StkFrames& frames = data->generator.tick();
  for ( unsigned int i=0; i < data->frames.size(); ++i ) {
    for(size_t n = 0; n < data->num_channels; ++n)
      *samples++ = data->frames[i];
  }
  /*
  for(size_t trackIdx = 0; trackIdx < sequencer->numTracks(); ++trackIdx) {
    StkFloat *samples = (StkFloat *) outputBuffer;
    stk::StkFrames& frames = sequencer->tick(data->frames, trackIdx, timeIdx);
    for ( unsigned int i=0; i<frames.size(); i++ ) {
      if(trackIdx == 0){
        for(int i=0; i < data->num_channels; ++i) {
          *samples++ = frames[i];
        }
      } else {
        for(int i=0; i < data->num_channels; ++i) {
          *samples++ += frames[i];
        }
      }
    }
  }
  */

  if ( data->counter > 80000 )
    data->done = true; 

  return 0;
}

static uint8_t BASS[16] = {
  220, 210, 200, 190, 180, 170, 160, 150,
  225, 215, 205, 195, 185, 175, 165, 155
};

static uint8_t DRUM[16] = {
  0b01010101,
  0b01010111,
  0b01010101,
  0b01010111,
  0b01010101,
  0b01010111,
  0b01010101,
  0b01010111
};

GLFWwindow* openWindow(size_t width, size_t height)
{
  //glfwWindowHint(GLFW_DECORATED, false);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  GLFWwindow* window = glfwCreateWindow(width,height,"Jivaro",NULL,NULL);

   // window datas
  //glfwSetWindowUserPointer(window, this);

  // set current opengl context
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  //GetContextVersionInfos();
  // load opengl functions
  //GarchGLApiLoad();
  //pxr::GlfContextCaps::InitInstance();
  //pxr::GlfContextCaps const& caps = pxr::GlfContextCaps::GetInstance();
  /*
  CreateFontAtlas();
  InitializeIcons();

  // setup callbacks
  glfwSetWindowSizeCallback(_window, ResizeCallback);
  glfwSetMouseButtonCallback(_window, ClickCallback);
  glfwSetScrollCallback(_window, ScrollCallback);
  glfwSetKeyCallback(_window, KeyboardCallback);
  glfwSetCharCallback(_window, CharCallback);
  glfwSetCursorPosCallback(_window, MouseMoveCallback);
  glfwSetWindowFocusCallback(_window, FocusCallback);
  glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

  // create main splittable view
  _mainView = new View(NULL, pxr::GfVec2f(0,0), pxr::GfVec2f(_width, _height));
  _mainView->SetWindow(this);
  _splitter = new SplitterUI(_mainView);
    
  Resize(_width, _height);

  glGenVertexArrays(1, &_vao);

  GLSLProgram* pgm = InitShapeShader((void*)this);
  _tool.SetProgram(pgm);
  */
  // ui
  //SetupImgui();
  //glfwMakeContextCurrent(NULL);
  return window;
}

void draw(GLFWwindow* window)
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  glfwMakeContextCurrent(window);
  //ImGui::SetCurrentContext(_context);
  
  //glBindVertexArray(_vao);
  // start the imgui frame
  //ImGui_ImplOpenGL3_NewFrame();
  //ImGui_ImplGlfw_NewFrame();
  //ImGui::NewFrame();

  // render the imgui frame
  //ImGui::Render();
  
  glViewport(0, 0, width, height);

  glClearColor(
    (float)rand()/(float)RAND_MAX, 
    (float)rand()/(float)RAND_MAX,
    (float)rand()/(float)RAND_MAX, 
    1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  //glBindVertexArray(0);

  glFinish();
  glFlush();
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main()
{
  // OpenGL window
  glfwInit();
  GLFWwindow* window = openWindow(1200, 800);
  std::cout << "window : " << window << std::endl;

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
  data.frames = stk::StkFrames(bufferFrames, data.num_channels);
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), 
    &bufferFrames, &tick, (void *)&data );
  }
  catch ( RtAudioError& error ) {
    error.printMessage();
    return 0;
  }


  data.sequencer.setLength(16);
  data.sequencer.addTrack();
  for(size_t t = 0; t < 16; ++t)
    data.sequencer.setTime(0, t, BASS[t]);
/*
  data.sequencer.addTrack();
  for(size_t t = 0; t < 16; ++t)
    data.sequencer.setTime(0, t, DRUM[t]);
*/
  data.base_frequency = 96;
  /*
  data.lfo.setFrequency(64.0);
  data.frequency.setFrequency(1);
  data.generator.setFrequency(data.base_frequency);
  data.blit.setHarmonics(7);
  */
  data.sequencer.start();

  try {
    dac.startStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
    return 0;
  }
  
  while (!glfwWindowShouldClose(window)) {
   Stk::sleep( 100 );
   draw(window);
  }
    
  // Shut down the callback and output stream.
  try {
    dac.closeStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
  }
}