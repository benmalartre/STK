
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Dear ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <SineWave.h>
#include <Blit.h>
#include <Bowed.h>
#include <Plucked.h>
#include <RtAudio.h>
#include "generator.h"
#include "sequencer.h"
using namespace stk;

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
  for ( unsigned int i=0; i < data->frames.size(); ++i) {
    //for(size_t n = 0; n < data->num_channels; ++n) {
      *samples++ = data->frames[i];
      //samples++;
    //}
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
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
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
  // start the imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static float f = 0.0f;
  static bool show_test_window = true;
  static bool show_another_window = false;
  static ImVec4 clear_color = ImColor(114, 144, 154);

  // Rendering
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  
  ImGui::Text("Hello, world!");
  
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(display_w, display_h));
  ImGui::Begin("window");
  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  ImGui::ColorEdit3("clear color", (float*)&clear_color);
  if (ImGui::Button("Test Window")) show_test_window ^= 1;
  if (ImGui::Button("Another Window")) show_another_window ^= 1;
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  
  ImGui::End();
  
  ImGui::Render();

  glViewport(0, 0, display_w, display_h);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main()
{
  // OpenGL window
  glfwInit();
  GLFWwindow* window = openWindow(1200, 800);
  glfwMakeContextCurrent(window);
  //gl3wInit();
  // Setup ImGui binding
  IMGUI_CHECKVERSION();
  ImGuiContext* context = ImGui::CreateContext();
  ImGui::SetCurrentContext(context);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 ");
  

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
  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();
}