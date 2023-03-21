#include "common.h"
#include "oscillator.h"
#include "lfo.h"
#include "adsr.h"
#include "random.h"
#include "filter.h"
#include "effect.h"
#include "arythmetic.h"
#include "sequencer.h"
#include "graph.h"

#include <GLFW/glfw3.h>

TxSequencer* sequencer;
stk::StkFrames frames;
std::vector<TxGraph*> graphs;

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;
  TxGraph* graph = graphs[0];

  TxTime& time = TxTime::instance();
  for ( unsigned int i=0; i < nBufferFrames; ++i) {
    const float sample = graph->tick();
    *samples++ = sample;
    *samples++ = sample;
    time.increment();
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
  
  GLFWwindow* window = glfwCreateWindow(width,height,"BendoW",NULL,NULL);

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

void setupStyle()
{
  ImGuiStyle& style = ImGui::GetStyle();
  //ImGui::StyleColorsLight(style);
  
	style.GrabRounding = 0.f;
	style.WindowRounding = 0.f;
	style.ScrollbarRounding = 3.f;
	style.FrameRounding = 3.f;
	style.WindowTitleAlign = ImVec2(0.5f,0.5f);

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

  //ImGui::SetNextWindowPos(ImVec2(0, 0));
  //ImGui::SetNextWindowSize(ImVec2(display_w, display_h));

  /*
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
  */

  //drawPlot(display_w, display_h);
  sequencer->draw();
  for(auto& graph: graphs)graph->draw();

  ImGui::ShowDemoWindow();
  ImPlot::ShowDemoWindow();
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

  // Setup ImGui binding
  IMGUI_CHECKVERSION();
  ImGuiContext* context = ImGui::CreateContext();
  ImPlotContext* plotContext = ImPlot::CreateContext();
  
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 ");

  setupStyle();
  

  // Set the global sample rate and rawwave path before creating class instances.
  stk::Stk::setSampleRate( 44100.0 );
  stk::Stk::setRawwavePath( "../../rawwaves/" );

  RtAudio dac;

  // Figure out how many bytes in an StkFloat and setup the RtAudio stream.
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
  catch ( RtAudioError& error ) {
    error.printMessage();
    return 0;
  }

  sequencer = new TxSequencer("Sequencer");
  sequencer->setLength(16);
  for(size_t t = 0; t < 16; ++t)
    sequencer->setBeat(t, {1, BASS[t]});
  sequencer->start();

  TxGraph* graph = new TxGraph("Graph");
  TxOscillator* oscillator = new TxOscillator("Oscillator");
  graph->addNode(oscillator);
  oscillator->setHarmonics(7);

  TxAdsr* adsr = new TxAdsr("Adsr");
  graph->addNode(adsr);
  adsr->connect(sequencer, "Trigger");
  oscillator->connect(sequencer, "Frequency", 1);

  oscillator->connect(adsr, "Envelope");

  TxLfo* lfo = new TxLfo("Lfo");
  graph->addNode(lfo);
  lfo->setFrequency(0.01f);
  lfo->setAmplitude(5.f);
  lfo->setOffset(6.f);

  TxArythmetic* arythmetic = new TxArythmetic("Arythmetic");
  graph->addNode(arythmetic);
  arythmetic->connect(sequencer, "Input1", 1);
  arythmetic->connect(lfo, "Input2");

  oscillator->connect(arythmetic, "Frequency");

  TxFilter* filter = new TxFilter("Filter");
  filter->connect(oscillator, "Samples");
  graph->addNode(filter);
  /*
  TxEffect* chorus = new TxEffect("Effect");
  chorus->connect(oscillator, "Samples");
  graph->addNode(chorus);
  */

  graphs.push_back(graph);
  /*
  TxGraph* graph1 = new TxGraph("Graph1");
  graphs.push_back(graph1);
  TxOscillator* bass = new TxOscillator("Bass");
  bass->setFrequency(110.f);
  bass->setWaveForm(TxOscillator::BLITSQUARE);
  bass->setHarmonics(7);
  
  TxParameter* frequency = bass->getParameter("Frequency");
  TxParameter* harmonics = bass->getParameter("Harmonics");

  TxLfo* fLfo = new TxLfo("BassFrequencyLfo");
  fLfo->setFrequency(1.f);
  fLfo->setAmplitude(10.f);
  fLfo->setOffset(60.f);
  frequency->connect(fLfo);

  TxLfo* hLfo = new TxLfo("BassHarmonicsLfo");
  hLfo->setFrequency(0.1f);
  hLfo->setAmplitude(5.f);
  hLfo->setOffset(7.f);
  harmonics->connect(hLfo);

  graph1->addNode(fLfo);
  graph1->addNode(hLfo);
  graph1->addNode(bass);  

  TxGraph* graph2 = new TxGraph("Graph2");
  graphs.push_back(graph2);
  TxOscillator* drum = new TxOscillator("Drum");
  drum->setFrequency(60.f);
  drum->setWaveForm(TxOscillator::BLITSAW);
  drum->setHarmonics(5);

  TxRandom* random = new TxRandom("DrumRandomFrequency");
  random->setMinimum(20.f);
  random->setMaximum(220.f);
  random->setFrequency(1.f);
  drum->connect(random, "Frequency");

  graph2->addNode(random);
  graph2->addNode(drum);
  */
  //drum->setWaveForm(4);
  //drum->setFrequency(88.f);
  //for(size_t t = 0; t < 16; ++t)
  //  track2->setTime(1, t, DRUM[t]);

  try {
    dac.startStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
    return 0;
  }
  
  while (!glfwWindowShouldClose(window)) {
    stk::Stk::sleep( 10 );
    draw(window);
  }
    
  // Shut down the callback and output stream.
  try {
    dac.closeStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
  }
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImPlot::DestroyContext(plotContext);
  ImGui::DestroyContext(context);
  
  glfwTerminate();
}