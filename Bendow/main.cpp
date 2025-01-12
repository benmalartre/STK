#include "common.h"
#include "sequencer.h"
#include "graph.h"
#include "factory.h"
#include "editor.h"

#include <GLFW/glfw3.h>

bool dragSplitter = false;
bool splitterHovered = false;

TxGraphEditor* graphEditor;
TxSequencerEditor* sequencerEditor;
TxSequencer* sequencer;
TxTrack* currentTrack = NULL;
stk::StkFrames frames;

ImFont* TX_FONT_BASE = NULL;
ImFont* TX_FONT_TITLE = NULL;


void 
SizeCallback(GLFWwindow* window, int width, int height)
{  
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize.x = width;
  io.DisplaySize.y = height;
  graphEditor->resize(SplitterHeight);
  sequencerEditor->resize(SplitterHeight);
}


void 
MouseMoveCallback(GLFWwindow* window, double x, double y)
{
  splitterHovered = std::abs(y - SplitterHeight) < 8;

  if(dragSplitter) {
    SplitterHeight = y;
    graphEditor->resize(SplitterHeight);
    sequencerEditor->resize(SplitterHeight);
  }
}
 
void 
ClickCallback(GLFWwindow* window, int button, int action, int mods)
{ 
  //ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

  if(action == GLFW_PRESS) {
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      if(std::abs(y - SplitterHeight) < 4)dragSplitter=true;
    }
  } else if(action == GLFW_RELEASE) {
    dragSplitter = false;
  } 
}

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;

  TxTime& time = TxTime::instance();
  for (unsigned int i = 0; i < nBufferFrames; ++i) {
    if(sequencer->running()) {
      const float sample = sequencer->tick(0);
      *samples++ = sample;
      *samples++ = sample;
      time.increment();
    } else {
      *samples++ = 0.f;
      *samples++ = 0.f;
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

  return 0;
}

static uint8_t BASS[16] = {
  110, 20, 20, 10, 110, 20, 20, 10,
  110, 30, 110, 20, 110, 55, 55, 33
};

static uint8_t DRUM[16] = {
  0, 0, 200, 0, 0, 0, 200, 200,
  0, 0, 200, 0, 0, 0, 200, 200
};

GLFWwindow* openWindow(size_t width, size_t height)
{
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
  
  GLFWwindow* window = glfwCreateWindow(width,height,"bendow",NULL,NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetWindowSizeCallback(window, SizeCallback);
  glfwSetMouseButtonCallback(window, ClickCallback);
  glfwSetCursorPosCallback(window, MouseMoveCallback);

  return window;
}

void setupStyle()
{
  ImGuiStyle& style = ImGui::GetStyle();
  //ImGui::StyleColorsLight(&style);
  
  style.GrabRounding = 2.f;
  style.ChildRounding = 8.f;
  style.ChildBorderSize = 4.f;
  style.FramePadding = ImVec2(2, 2);
  style.WindowPadding = ImVec2(4, 4);
  style.WindowRounding = 0.f;
  style.ScrollbarRounding = 3.f;
  style.FrameRounding = 3.f;
  style.WindowTitleAlign = ImVec2(0.5f,0.5f);
  style.Alpha = 1.f;

  style.Colors[ImGuiCol_Text]                  = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
  style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
  style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
  style.Colors[ImGuiCol_Border]                = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
  style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
  style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
  style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
  style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
  style.Colors[ImGuiCol_Button]                = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
  style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_Header]                = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
  style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
 
}

int main()
{
  // OpenGL window
  glfwInit(); 

  GLFWwindow* window = openWindow(1200, 800);
  glfwMakeContextCurrent(window);

  // Setup ImGui binding
  IMGUI_CHECKVERSION();
  ImGuiContext* context = ImGui::CreateContext();
  ImPlotContext* plotContext = ImPlot::CreateContext();
  
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 ");

  ImGuiIO& io = ImGui::GetIO();
  //io.Fonts->AddFontDefault();
  TX_FONT_BASE = io.Fonts->AddFontFromFileTTF("fonts/tahoma.ttf", 13);
  TX_FONT_TITLE = io.Fonts->AddFontFromFileTTF("fonts/tahomabd.ttf", 24);

  setupStyle();

  // start an audio stream
  stk::Stk::setSampleRate( 44100.0 );
  stk::Stk::setRawwavePath( "../../rawwaves/" );

  RtAudio dac;
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = TX_NUM_CHANNELS;
  frames.resize(stk::RT_BUFFER_SIZE, TX_NUM_CHANNELS);
  RtAudioFormat format = 
    ( sizeof(stk::StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  unsigned int bufferFrames = stk::RT_BUFFER_SIZE;  
  
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)stk::Stk::sampleRate(), 
    &bufferFrames, &tick, (void *)&frames );
  }
  catch (stk::StkError& error ) {
    error.printMessage();
    return 0;
  }

  // create the sequencer
  TxFactory::initialize();
  sequencer = new TxSequencer(2);
  sequencer->setLength(16);
  for (size_t t = 0; t < 16; ++t) {
    sequencer->setBeat(0, t, { 1, BASS[t] });
    sequencer->setBeat(1, t, { 4, DRUM[t] });
  }
  sequencer->start();

  currentTrack = sequencer->track(0);

  // setup the editors
  sequencerEditor = new TxSequencerEditor(sequencer);
  graphEditor = new TxGraphEditor(currentTrack->graph());

  sequencerEditor->resize(SplitterHeight);
  graphEditor->resize(SplitterHeight);

  graphEditor->setCurrent(currentTrack);
  sequencerEditor->setCurrent(currentTrack);


  // start the audio backkground stream
  try {
    dac.startStream();
  }
  catch ( stk::StkError &error ) {
    error.printMessage();
    return 0;
  }


  static ImVec4 clear_color = ImColor(220, 220, 220);

  // Rendering
  int display_w, display_h;
  glfwGetWindowSize(window, &display_w, &display_h);
  SizeCallback(window, display_w, display_h);

  // main loop audio rendering in background
  while (!glfwWindowShouldClose(window)) {
    stk::Stk::sleep( 10 );

     // start the imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::SetMouseCursor(splitterHovered ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_Arrow);

    graphEditor->setCurrent(sequencerEditor->getCurrentTrackIndex());

    sequencerEditor->draw();
    graphEditor->draw();
    
    ImGui::Render();

    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
    
  // Shut down the callback and output stream.
  try {
    dac.closeStream();
  }
  catch (stk::StkError &error ) {
    error.printMessage();
  }
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImPlot::DestroyContext(plotContext);
  ImGui::DestroyContext(context);
  
  glfwTerminate();
}