#include "common.h"
#include "generator.h"
#include "lfo.h"
#include "random.h"
#include "sequencer.h"

TxSequencer sequencer;
stk::StkFrames frames;
std::vector<TxNode*> nodes;

constexpr size_t TX_BUFFER_SIZE = 64 * stk::RT_BUFFER_SIZE;
constexpr size_t TX_BUFFER_OFFSET = stk::RT_BUFFER_SIZE;
stk::StkFloat buffer[TX_NUM_CHANNELS][TX_BUFFER_SIZE];

static void shiftBuffers() {
  for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
    for(size_t i = 0; i < TX_BUFFER_SIZE - TX_BUFFER_OFFSET; ++i) {
      buffer[n][i] = buffer[n][i + TX_BUFFER_OFFSET];
    }
  }
}

static void feedBuffers(const stk::StkFrames& frames) {
  shiftBuffers();
  for(size_t n = 0; n < TX_NUM_CHANNELS; ++n) {
    for(size_t i = 0; i < TX_BUFFER_OFFSET; ++i) {
      buffer[n][TX_BUFFER_SIZE - i - 1] = frames[i + n];
    }
  }
}

int tick( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;
  stk::StkFrames& frames = *(stk::StkFrames*)userData;
  sequencer.tick(frames, 0);
  
  feedBuffers(frames);
  for ( unsigned int i=0; i < frames.size(); ++i) {
    *samples++ = frames[i];
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
  
  GLFWwindow* window = glfwCreateWindow(width,height,"7ound",NULL,NULL);

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

void drawPlot(int width, int height) {
  ImGui::Begin("Plot Window", NULL);
  ImGui::PlotConfig conf;
  //conf.values.xs = x_data; // this line is optional
  conf.values.ys = &buffer[0][0];
  conf.values.count = TX_BUFFER_SIZE;
  conf.scale.min = -3;
  conf.scale.max = 1;
  conf.tooltip.show = true;
  conf.tooltip.format = "x=%.2f, y=%.2f";
  conf.grid_x.show = false;
  conf.grid_y.show = false;
  conf.grid_x.size = 128;
  conf.grid_y.size = 0.5;
  conf.frame_size = ImVec2(width, height);
  conf.line_thickness = 4.f;

  ImGui::Plot("plot", conf);
  ImGui::End();
}

void setupStyle( bool bStyleDark_, float alpha_  )
{
  ImGuiStyle& style = ImGui::GetStyle();

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
  sequencer.draw();
  for(auto& node: nodes) {
    node->draw();
  }
  
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
  ImGui::SetCurrentContext(context);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330 ");

  setupStyle(false, 0.5f);
  

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

  sequencer.setLength(16);
  TxSequencer::Track* track1 = sequencer.addTrack("Track1");
  TxGenerator* bass = new TxGenerator("Bass");
  nodes.push_back(bass);
  bass->setFrequency(110.f);
  bass->setWaveForm(TxGenerator::BLITSQUARE);
  bass->setHarmonics(7);
  track1->setNode(bass);
  
  TxParameter* frequency = bass->getParameter("Frequency");
  TxParameter* harmonics = bass->getParameter("Harmonics");

  TxLfo* fLfo = new TxLfo("BassFrequencyLfo");
  nodes.push_back(fLfo);
  fLfo->setFrequency(1.f);
  fLfo->setAmplitude(10.f);
  fLfo->setOffset(60.f);
  frequency->connect(fLfo);

  TxLfo* hLfo = new TxLfo("BassHarmonicsLfo");
  nodes.push_back(hLfo);
  hLfo->setFrequency(0.1f);
  hLfo->setAmplitude(5.f);
  hLfo->setOffset(7.f);
  harmonics->connect(hLfo);
  
  //bass->setFrequency(60.f);
  for(size_t t = 0; t < 16; ++t)
    sequencer.setTime(0, t, BASS[t]);

  TxSequencer::Track* track2 = sequencer.addTrack("Track2");
  TxGenerator* drum = new TxGenerator("Drum");
  nodes.push_back(drum);
  drum->setFrequency(60.f);
  drum->setWaveForm(TxGenerator::BLITSAW);
  drum->setHarmonics(5);
  track2->setNode(drum);

  TxRandom* random = new TxRandom("DrumRandomFrequency");
  nodes.push_back(random);
  random->setMinimum(20.f);
  random->setMaximum(220.f);
  random->setFrequency(1.f);
  drum->connect(random, "Frequency");
  //drum->setWaveForm(4);
  //drum->setFrequency(88.f);
  for(size_t t = 0; t < 16; ++t)
    sequencer.setTime(1, t, DRUM[t]);

  sequencer.start();

  try {
    dac.startStream();
  }
  catch ( RtAudioError &error ) {
    error.printMessage();
    return 0;
  }
  
  while (!glfwWindowShouldClose(window)) {
    stk::Stk::sleep( 100 );
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