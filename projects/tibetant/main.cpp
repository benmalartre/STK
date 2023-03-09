
#include <GL/gl3w.h>
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

// initialize
void initGL()
{
	int err = gl3wInit();
	// initialize gl3w
	if (err)
	{
		switch (err)
		{
		case  GL3W_ERROR_LIBRARY_OPEN:
			std::cerr << "GL3W_ERROR_LIBRARY_OPEN" << std::endl;
			return;
		case GL3W_ERROR_INIT:
			std::cerr << "GL3W_ERROR_INIT" << std::endl;
			return;
		case GL3W_ERROR_OPENGL_VERSION:
			std::cerr << "GL3W_ERROR_OPENGL_VERSION" << std::endl;
			return;
		default:
			std::cerr << "GL3W_ERROR_UNKNOWN" << std::endl;
			return;
		}
	}
	else std::cerr << "GL3W_INITIALIZED!!!" << std::endl;
}

void termGL()
{
	std::cerr << "Should Term LIB GL..."  << std::endl;
	gl3wTerm();
	//close_libgl();
}

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
  sequencer->tick(data->frames);
  //stk::StkFrames& frames = data->generator.tick();
  for ( unsigned int i=0; i < data->frames.size(); ++i ) {
    for(size_t n = 0; n < data->num_channels; ++n) {
      *samples++ = data->frames[i];
    }
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
  110, 0, 0, 0, 110, 0, 0, 0,
  110, 0, 0, 0, 110, 0, 55, 0
};

static uint8_t DRUM[16] = {
  0, 0, 200, 0, 0, 0, 200, 200,
  0, 0, 200, 0, 0, 0, 200, 200
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
  //glfwSwapInterval(1);

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
  glfwPollEvents();
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

  glfwMakeContextCurrent(window);

	if (gl3wInit()) {
		std::cerr << "failed to initialize OpenGL" << std::endl;
		return -1;
	}
	if (!gl3wIsSupported(3, 2)) {
		std::cerr << "OpenGL 3.2 not supported" << std::endl;
		return -1;
	}
	std::cerr << "OpenGL " <<  glGetString(GL_VERSION) <<", GLSL " << 
	       glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

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
  data.frames.resize(RT_BUFFER_SIZE, data.num_channels);
  RtAudioFormat format = 
    ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  unsigned int bufferFrames = RT_BUFFER_SIZE;

  std::cout << "!nitialized data" << std::endl;
  
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), 
    &bufferFrames, &tick, (void *)&data );
  }
  catch ( RtAudioError& error ) {
    error.printMessage();
    return 0;
  }

  std::cout << "stk opened stream" << std::endl;
  data.sequencer.setLength(16);
  std::cout << "stk sequencer set length" << std::endl;
  Sequencer::Track* bass = data.sequencer.addTrack();
  std::cout << "stk sequencer bass " << bass << std::endl;
  for(size_t t = 0; t < 16; ++t)
    data.sequencer.setTime(0, t, BASS[t]);
/*
  Sequencer::Track* drum = data.sequencer.addTrack();
  drum->setWaveForm(3);
  for(size_t t = 0; t < 16; ++t)
    data.sequencer.setTime(1, t, DRUM[t]);
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

  glfwTerminate();
  gl3wTerm();
}